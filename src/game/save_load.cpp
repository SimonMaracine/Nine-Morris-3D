#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/save_load.h"
#include "game/piece.h"
#include "game/node.h"
#include "game/undo_redo_state.h"
#include "game/boards/generic_board.h"
#include "game/boards/standard_board.h"

/*
Unserialized fields:
    vertex_array, material
*/
template<typename Archive>
void serialize(Archive& archive, Renderer::Model& model) {
    archive(
        model.position,
        model.rotation,
        model.scale,
        model.index_count,
        model.outline_color,
        model.id,
        model.cast_shadow
    );
}

/*
Unserialized fields:
    texture
*/
template<typename Archive>
void serialize(Archive& archive, Renderer::Quad& quad) {
    archive(quad.position, quad.scale);
}

template<typename Archive>
void serialize(Archive& archive, Camera& camera) {
    archive(
        camera.sensitivity,
        camera.position,
        camera.pitch,
        camera.yaw,
        camera.view_matrix,
        camera.projection_matrix,
        camera.projection_view_matrix,
        camera.point,
        camera.distance_to_point,
        camera.angle_around_point,
        camera.x_velocity,
        camera.y_velocity,
        camera.zoom_velocity,
        camera.auto_move_x,
        camera.target_angle_around_point,
        camera.auto_x_velocity,
        camera.virtual_angle_around_point,
        camera.auto_move_y,
        camera.target_pitch,
        camera.auto_y_velocity,
        camera.virtual_pitch,
        camera.auto_move_zoom,
        camera.target_distance_to_point, 
        camera.auto_zoom_velocity,
        camera.virtual_distance_to_point,
        camera.cached_towards_position,
        camera.dont_auto_call_go_towards_position
    );
}

/*
Unserialized fields:
    hovered_node, hovered_piece, selected_piece, undo_redo_state, keyboard, game_context
*/
template<typename Archive>
void serialize(Archive& archive, GenericBoard& board) {
    archive(
        board.model,
        board.paint_model,
        board.nodes,
        board.pieces,
        board.phase,
        board.turn,
        board.ending,
        board.hovered_node,
        board.hovered_piece,
        board.selected_piece,
        // board.white_pieces_count,
        // board.black_pieces_count,
        // board.not_placed_pieces_count,
        // board.must_take_piece,
        board.can_jump,
        // board.turns_without_mills,
        board.repetition_history,
        board.next_move,
        board.is_players_turn,
        board.switched_turn
    );
}

template<typename Archive>
void serialize(Archive& archive, StandardBoard& board) {
    archive(
        cereal::virtual_base_class<GenericBoard>(&board),
        board.white_pieces_count,
        board.black_pieces_count,
        board.not_placed_pieces_count,
        board.must_take_piece,
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
void serialize(Archive& archive, UndoRedoState& undo_redo_state) {
    archive(undo_redo_state.undo, undo_redo_state.redo);
}

template<typename Archive>
void serialize(Archive& archive, UndoRedoState::State& state) {
    archive(state.board, state.camera, state.game_state);
}

template<typename Archive>
void serialize(Archive& archive, Piece& piece) {
    archive(
        piece.index,
        piece.type,
        piece.in_use,
        piece.model,
        piece.node,
        piece.movement,
        piece.show_outline,
        piece.to_take,
        piece.pending_remove,
        piece.selected
    );
}

template<typename Archive>
void serialize(Archive& archive, Piece::Movement& movement) {
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
void serialize(Archive& archive, Node& node) {
    archive(node.index, node.model, node.piece);
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

namespace save_load {
    template<typename Archive>
    void serialize(Archive& archive, SavedGame& saved_game) {
        archive(
            saved_game.board,
            saved_game.camera,
            saved_game.time,
            saved_game.date,
            saved_game.undo_redo_state,
            saved_game.white_player,
            saved_game.black_player
        );
    }

    static std::string get_file_path() noexcept(false) {
        std::string file_path;

        try {
            file_path = paths::path_for_saved_data(SAVE_GAME_FILE);
        } catch (const user_data::UserNameError& e) {
            throw SaveFileError(e.what());
        }

        return file_path;
    }

    void save_game_to_file(const SavedGame& saved_game) noexcept(false) {
        const std::string file_path = get_file_path();

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file '" + std::string(SAVE_GAME_FILE) + "' for writing"
            );
        }

        try {
            cereal::BinaryOutputArchive output {file};
            output(saved_game);
        } catch (const std::exception& e) {  // Just to be sure...
            throw SaveFileError(e.what());
        }

        DEB_INFO("Saved game to file '{}'", SAVE_GAME_FILE);
    }

    void load_game_from_file(SavedGame& saved_game) noexcept(false) {
        const std::string file_path = get_file_path();

        std::ifstream file {file_path, std::ios::binary};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file '" + std::string(SAVE_GAME_FILE) + "'"
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

        DEB_INFO("Loaded game from file '{}'", SAVE_GAME_FILE);
    }

    void delete_save_game_file(std::string_view file_path) {
        if (remove(file_path.data()) != 0) {
            REL_INFO("Could not delete save game file '{}'", file_path);
        } else {
            REL_INFO("Deleted save game file '{}'", file_path);
        }
    }

    void handle_save_file_not_open_error(std::string_view app_name) {
        bool user_data_directory;

        try {
            user_data_directory = user_data::user_data_directory_exists(app_name);
        } catch (const user_data::UserNameError& e) {
            REL_ERROR("{}", e.what());
            return;
        }

        if (!user_data_directory) {
            REL_INFO("User data folder missing; creating one...");

            try {
                const bool success = user_data::create_user_data_directory(app_name);
                if (!success) {
                    REL_ERROR("Could not create user data directory");
                    return;
                }
            } catch (const user_data::UserNameError& e) {
                REL_ERROR("{}", e.what());
                return;
            }
        }
    }
}
