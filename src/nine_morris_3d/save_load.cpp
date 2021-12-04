#include <fstream>
#include <stdio.h>

#include <glm/glm.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>

#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/node.h"
#include "opengl/renderer/camera.h"
#include "other/logging.h"

#define SAVE_GAME_FILE "last_game.dat"

/*
Unserialized variables:
    vertex_array, diffuse_texture, hovered_node, hovered_piece, selected_piece, state_history
*/
template<typename Archive>
void serialize(Archive& archive, Board& board) {
    archive(board.id, board.scale, board.index_count, board.specular_color, board.shininess,
            board.nodes, board.pieces, board.phase, board.turn, board.ending, board.white_pieces_count,
            board.black_pieces_count, board.not_placed_white_pieces_count,
            board.not_placed_black_pieces_count, board.should_take_piece, board.can_jump,
            board.turns_without_mills, board.repetition_history, board.paint, board.next_move);
}

/*
Unserialized variables:
    vertex_array, diffuse_texture
*/
template<typename Archive>
void serialize(Archive& archive, BoardPaint& paint) {
    archive(paint.position, paint.scale, paint.index_count, paint.specular_color, paint.shininess);
}

template<typename Archive>
void serialize(Archive& archive, ThreefoldRepetitionHistory& repetition_history) {
    archive(repetition_history.ones, repetition_history.twos);
}

/*
Unserialized variables:
    vertex_array, diffuse_texture, node
*/
template<typename Archive>
void serialize(Archive& archive, Piece& piece) {
    archive(piece.id, piece.position, piece.rotation, piece.scale, piece.velocity, piece.target,
            piece.should_move, piece.distance_travelled, piece.distance_to_travel,
            piece.index_count, piece.specular_color, piece.shininess, piece.select_color,
            piece.hover_color, piece.type, piece.in_use, piece.node_id, piece.show_outline,
            piece.to_take, piece.pending_remove, piece.selected, piece.active);
}

/*
Unserialized variables:
    vertex_array, piece
*/
template<typename Archive>
void serialize(Archive& archive, Node& node) {
    archive(node.id, node.position, node.scale, node.index_count, node.piece_id, node.index);
}

template<typename Archive>
void serialize(Archive& archive, Camera& camera) {
    archive(camera.position, camera.pitch, camera.yaw, camera.view_matrix, camera.projection_matrix,
            camera.projection_view_matrix, camera.point, camera.distance_to_point,
            camera.angle_around_point, camera.x_velocity, camera.y_velocity, camera.zoom_velocity);
}

namespace glm {
    template<typename Archive>
    void serialize(Archive& archive, vec3& vector) {
        archive(vector.x, vector.y, vector.z);
    }

    template<typename Archive>
    void serialize(Archive& archive, mat4& matrix) {
        archive(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
                matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
                matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
                matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
    }
}

namespace save_load {
    static bool file_exists(const std::string& file_path) {
        FILE* file = fopen(file_path.c_str(), "r");
        if (file) {
            fclose(file);
            return true;
        } else {
            return false;
        }
    }

    void save_game(const GameState& game_state) {
        std::ofstream file = std::ofstream(SAVE_GAME_FILE, std::ios::out | std::ios::binary | std::ios::trunc);

        if (!file.is_open()) {
            spdlog::error("Could not open the last game file for writing '{}'", SAVE_GAME_FILE);
            return;
        }

        cereal::BinaryOutputArchive output{file};
        output(game_state);

        file.close();

        SPDLOG_INFO("Saved game to file '{}'", SAVE_GAME_FILE);
    }

    void load_game(GameState& game_state) {
        std::ifstream file = std::ifstream(SAVE_GAME_FILE, std::ios::in | std::ios::binary);

        if (!file.is_open()) {
            spdlog::error("Could not open the last game file '{}'", SAVE_GAME_FILE);
            return;
        }

        cereal::BinaryInputArchive input{file};
        input(game_state);

        file.close();

        SPDLOG_INFO("Loaded game from file '{}'", SAVE_GAME_FILE);
    }

    bool save_files_exist() {
        if (file_exists(SAVE_GAME_FILE)) {
            return true;
        } else {
            SPDLOG_ERROR("Save file is either missing or is inaccessible: '{}'", SAVE_GAME_FILE);
            return false;
        }
    }
}
