#pragma once

#include <engine/engine_other.h>

#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/entities/board.h"
#include "game/entities/serialization/board_serialized.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/entities/serialization/jump_board_serialized.h"
#include "game/entities/serialization/piece_serialized.h"
#include "game/entities/serialization/node_serialized.h"
#include "game/undo_redo_state.h"
#include "game/piece_movement.h"
#include "game/point_camera_controller.h"
#include "game/game_options.h"
#include "launcher/launcher_options.h"
#include "other/constants.h"

namespace save_load {
    static constexpr const char* NO_LAST_GAME = "No Last Game";

    class SaveFileError : public std::runtime_error {
    public:
        explicit SaveFileError(const std::string& message)
            : std::runtime_error(message) {}
        explicit SaveFileError(const char* message)
            : std::runtime_error(message) {}
    };

    class SaveFileNotOpenError : public SaveFileError {
    public:
        SaveFileNotOpenError(const std::string& message)
            : SaveFileError(message) {}
        SaveFileNotOpenError(const char* message)
            : SaveFileError(message) {}
    };

    void handle_save_file_not_open_error();
    std::string save_game_file_name(std::string_view name);

    template<typename B>
    struct SavedGame {
        B board_serialized;
        PointCameraController camera_controller;
        unsigned int time = 0;  // In deciseconds
        std::string date = NO_LAST_GAME;
        UndoRedoState<B> undo_redo_state;
        GamePlayer white_player = GamePlayer::None;
        GamePlayer black_player = GamePlayer::None;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(
                board_serialized,
                camera_controller,
                time,
                date,
                undo_redo_state,
                white_player,
                black_player
            );
        }
    };

    template<typename B>
    void save_game_to_file(const SavedGame<B>& saved_game, std::string_view file_name) noexcept(false) {
        const std::string file_path = file_system::path_for_saved_data(file_name);

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file `" + std::string(file_name) + "` for writing"
            );
        }

        try {
            cereal::BinaryOutputArchive output {file};
            output(saved_game);
        } catch (const std::exception& e) {  // Just to be sure...
            throw SaveFileError(e.what());
        }

        DEB_INFO("Saved game to file `{}`", file_name);
    }

    template<typename B>
    void load_game_from_file(SavedGame<B>& saved_game, std::string_view file_name) noexcept(false) {
        const std::string file_path = file_system::path_for_saved_data(file_name);

        std::ifstream file {file_path, std::ios::binary};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file `" + std::string(file_name) + "` for reading"
            );
        }

        try {
            cereal::BinaryInputArchive input {file};
            input(saved_game);
        } catch (const cereal::Exception& e) {
            throw SaveFileError(e.what());
        } catch (const std::exception& e) {
            throw SaveFileError(e.what());
        }

        DEB_INFO("Loaded game from file `{}`", file_name);
    }
}

// Mostly all serialization stuff

template<typename Archive>
void serialize(Archive& archive, PointCameraController& camera_controller) {
    archive(
        camera_controller.sensitivity,
        camera_controller.position,
        camera_controller.pitch,
        camera_controller.yaw,
        camera_controller.point,
        camera_controller.distance_to_point,
        camera_controller.angle_around_point
    );
}

template<typename Archive>
void serialize(Archive& archive, BoardSerialized& board) {
    archive(
        board.nodes,
        board.pieces,
        board.phase,
        board.turn,
        board.ending,
        board.must_take_piece,
        board.repetition_history,
        board.is_players_turn,
        board.turn_count
    );
}

template<typename Archive>
void serialize(Archive& archive, StandardBoardSerialized& board) {
    archive(
        cereal::base_class<BoardSerialized>(&board),
        board.can_jump,
        board.white_pieces_count,
        board.black_pieces_count,
        board.not_placed_white_pieces_count,
        board.not_placed_black_pieces_count,
        board.turns_without_mills
    );
}

template<typename Archive>
void serialize(Archive& archive, JumpBoardSerialized& board) {
    archive(
        cereal::base_class<BoardSerialized>(&board),
        board.turns_without_mills
    );
}

template<typename Archive>
void serialize(Archive& archive, ThreefoldRepetitionHistory& repetition_history) {
    archive(repetition_history.ones, repetition_history.twos);
}

template<typename Archive>
void serialize(Archive& archive, ThreefoldRepetitionHistory::PositionPlusInfo& position_plus_info) {
    archive(position_plus_info.position, position_plus_info.piece_index, position_plus_info.node_index);
}

template<typename Archive>
void serialize(Archive& archive, PieceSerialized& piece) {
    archive(
        piece.index,
        piece.type,
        piece.in_use,
        piece.position,
        piece.rotation,
        piece.node_index,
        piece.show_outline,
        piece.to_take,
        piece.pending_remove
    );
}

template<typename Archive>
void serialize(Archive& archive, NodeSerialized& node) {
    archive(node.index, node.piece_index);
}

template<typename Archive>
void serialize(Archive& archive, PieceMovement& movement) {
    archive(
        movement.type,
        movement.velocity,
        movement.target,
        movement.target0,
        movement.target1,
        movement.reached_target0,
        movement.reached_target1,
        movement.moving
    );
}

template<typename Archive>
void serialize(Archive& archive, BoardEnding& board_ending) {
    archive(board_ending.type, board_ending.message);
}

namespace glm {
    template<typename Archive>
    void serialize(Archive& archive, vec3& vector) {
        archive(vector.x, vector.y, vector.z);
    }

    template<typename Archive>
    void serialize(Archive& archive, mat4& matrix) {
        archive(
            matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
            matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
            matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
            matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]
        );
    }
}
