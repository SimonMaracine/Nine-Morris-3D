#include <fstream>
#include <string>
#include <exception>
#include <stdio.h>

#include <glm/glm.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>

#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/node.h"
#include "graphics/renderer/camera.h"
#include "other/logging.h"
#include "other/user_data.h"

#define SAVE_GAME_FILE "last_game.dat"

/*
Unserialized variables:
    vertex_array, diffuse_texture, hovered_node, hovered_piece, selected_piece
*/
template<typename Archive>
void serialize(Archive& archive, Board& board) {
    archive(board.id, board.scale, board.index_count, board.specular_color, board.shininess,
            board.nodes, board.pieces, board.phase, board.turn, board.ending, board.white_pieces_count,
            board.black_pieces_count, board.not_placed_white_pieces_count,
            board.not_placed_black_pieces_count, board.should_take_piece, board.can_jump,
            board.turns_without_mills, board.repetition_history, board.paint, board.state_history,
            board.next_move);
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
    archive(piece.id, piece.position, piece.rotation, piece.scale, piece.movement, piece.should_move,
            piece.index_count, piece.specular_color, piece.shininess, piece.select_color,
            piece.hover_color, piece.type, piece.in_use, piece.node_id, piece.show_outline,
            piece.to_take, piece.pending_remove, piece.selected, piece.active);
}

template<typename Archive>
void serialize(Archive& archive, Piece::Movement& movement) {
    archive(movement.type, movement.velocity, movement.target, movement.target0, movement.target1,
            movement.reached_target0, movement.reached_target1);
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
    archive(camera.sensitivity, camera.position, camera.pitch, camera.yaw, camera.view_matrix,
            camera.projection_matrix, camera.projection_view_matrix, camera.point,
            camera.distance_to_point, camera.angle_around_point, camera.x_velocity,
            camera.y_velocity, camera.zoom_velocity);
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
    static std::string path(const char* file) {  // Throws exception
#ifndef NDEBUG
        // Use relative path for both operating systems
        return std::string(file);
#else
    #if defined(__GNUG__)
        std::string path = user_data::get_user_data_path() + file;
        return path;
    #elif defined(_MSC_VER)
        std::string path = user_data::get_user_data_path() + "\\" + file;
        return path;
    #else
        #error "GCC or MSVC must be used (for now)"
    #endif
#endif
    }

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
        std::string file_path;
        try {
            file_path = path(SAVE_GAME_FILE);
        } catch (const std::runtime_error& e) {
            REL_ERROR("{}", e.what());
            return;
        }

        std::ofstream file (file_path, std::ios::binary | std::ios::trunc);

        if (!file.is_open()) {
            REL_ERROR("Could not open the last game file '{}' for writing", SAVE_GAME_FILE);

            try {
                if (!user_data::create_user_data_directory()) {
                    REL_ERROR("Could not recreate user data directory");
                } else {
                    REL_INFO("Recreated user data directory");
                }
            } catch (const std::runtime_error& e) {
                REL_ERROR("{}", e.what());
                return;
            }

            return;
        }

        cereal::BinaryOutputArchive output{file};
        output(game_state);

        DEB_INFO("Saved game to file '{}'", SAVE_GAME_FILE);
    }

    void load_game(GameState& game_state) {  // Throws exception
        std::string file_path;
        file_path = path(SAVE_GAME_FILE);

        std::ifstream file (file_path, std::ios::binary);

        if (!file.is_open()) {
            std::string message = "Could not open last save game file '" + std::string(SAVE_GAME_FILE) + "'";
            throw std::runtime_error(message);
        }

        try {
            cereal::BinaryInputArchive input{file};
            input(game_state);
        } catch (const cereal::Exception& e) {
            REL_ERROR("Error reading save game file: {}, deleting it...", e.what());
            delete_save_file(SAVE_GAME_FILE);
            return;
        } catch (const std::exception& e) {
            REL_ERROR("Error reading save game file: {}, deleting it...", e.what());
            delete_save_file(SAVE_GAME_FILE);
            throw;
        }

        DEB_INFO("Loaded game from file '{}'", SAVE_GAME_FILE);
    }

    void delete_save_file(const std::string& file_path) {
        if (remove(file_path.c_str()) != 0) {
            REL_INFO("Could not delete save game file '{}'", file_path.c_str());
        } else {
            DEB_INFO("Deleted save game file '{}'", file_path.c_str());
        }
    }

    bool save_files_exist() {
        if (file_exists(SAVE_GAME_FILE)) {
            return true;
        } else {
            DEB_ERROR("Save game file is either missing or is inaccessible: '{}'", SAVE_GAME_FILE);
            return false;
        }
    }
}
