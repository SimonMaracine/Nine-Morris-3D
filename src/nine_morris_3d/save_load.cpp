#include <fstream>
#include <string>
#include <stdexcept>
#include <stdio.h>

#include <glm/glm.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>

#include "application/platform.h"
#include "graphics/renderer/new_renderer.h"
#include "graphics/renderer/camera.h"
#include "graphics/renderer/material.h"
#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/node.h"
#include "other/logging.h"
#include "other/user_data.h"

#define SAVE_GAME_FILE "last_game.dat"

/*
Unserialized variables:
    vertex_array, material, handle
*/
template<typename Archive>
void serialize(Archive& archive, Renderer::Model& model) {
    archive(model.index_count, model.position, model.rotation, model.scale, model.outline_color);
}

template<typename Archive>
void serialize(Archive& archive, Board& board) {
    archive(board.model, board.paint_model, board.nodes, board.pieces, board.phase, board.turn,
            board.ending, board.white_pieces_count, board.black_pieces_count,
            board.not_placed_white_pieces_count, board.not_placed_black_pieces_count,
            board.should_take_piece, board.can_jump, board.turns_without_mills, board.repetition_history,
            board.state_history, board.next_move);
}

template<typename Archive>
void serialize(Archive& archive, ThreefoldRepetitionHistory& repetition_history) {
    archive(repetition_history.ones, repetition_history.twos);
}

/*
Unserialized variables:
    node
*/
template<typename Archive>
void serialize(Archive& archive, Piece& piece) {
    archive(piece.id, piece.model, piece.movement, piece.should_move, piece.type, piece.in_use,
            piece.node_id, piece.show_outline, piece.to_take, piece.pending_remove, piece.selected,
            piece.active, piece.renderer_with_outline);
}

template<typename Archive>
void serialize(Archive& archive, Piece::Movement& movement) {
    archive(movement.type, movement.velocity, movement.target, movement.target0, movement.target1,
            movement.reached_target0, movement.reached_target1);
}

/*
Unserialized variables:
    piece
*/
template<typename Archive>
void serialize(Archive& archive, Node& node) {
    archive(node.id, node.model, node.piece_id, node.index);
}

template<typename Archive>
void serialize(Archive& archive, Camera& camera) {
    archive(camera.sensitivity, camera.position, camera.pitch, camera.yaw, camera.view_matrix,
            camera.projection_matrix, camera.projection_view_matrix, camera.point,
            camera.distance_to_point, camera.angle_around_point, camera.x_velocity,
            camera.y_velocity, camera.zoom_velocity, camera.auto_move_x, camera.target_angle_around_point,
            camera.auto_x_velocity, camera.virtual_angle_around_point, camera.auto_move_y,
            camera.target_pitch, camera.auto_y_velocity, camera.virtual_pitch, camera.auto_move_zoom,
            camera.target_distance_to_point, camera.auto_zoom_velocity, camera.virtual_distance_to_point,
            camera.cached_towards_position, camera.dont_auto_call_go_towards_position);
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
#if defined(NINE_MORRIS_3D_DEBUG)
        // Use relative path for both operating systems
        return std::string(file);
#elif defined(NINE_MORRIS_3D_RELEASE)
    #if defined(NINE_MORRIS_3D_LINUX)
        std::string path = user_data::get_user_data_directory_path() + file;
        return path;
    #elif defined(NINE_MORRIS_3D_WINDOWS)
        std::string path = user_data::get_user_data_directory_path() + "\\" + file;
        return path;
    #endif
#endif
    }

    void save_game_to_file(const GameState& game_state) {  // Throws exception
        std::string file_path;
        try {
            file_path = path(SAVE_GAME_FILE);
        } catch (const user_data::UserNameError& e) {
            throw SaveFileError(e.what());
        }

        std::ofstream file (file_path, std::ios::binary | std::ios::trunc);

        if (!file.is_open()) {
            std::string message = "Could not open last save game file '" + std::string(SAVE_GAME_FILE)
                    + "' for writing";
            throw SaveFileNotOpenError(message);
        }

        try {
            cereal::BinaryOutputArchive output {file};
            output(game_state);
        } catch (const std::exception& e) {  // Just to be sure...
            throw SaveFileError(e.what());
        }

        DEB_INFO("Saved game to file '{}'", SAVE_GAME_FILE);
    }

    void load_game_from_file(GameState& game_state) {  // Throws exception
        std::string file_path;
        try {
            file_path = path(SAVE_GAME_FILE);
        } catch (const user_data::UserNameError& e) {
            throw SaveFileError(e.what());
        }

        std::ifstream file (file_path, std::ios::binary);

        if (!file.is_open()) {
            std::string message = "Could not open last save game file '" + std::string(SAVE_GAME_FILE) + "'";
            throw SaveFileNotOpenError(message);
        }

        try {
            cereal::BinaryInputArchive input {file};
            input(game_state);
        } catch (const cereal::Exception& e) {
            throw SaveFileError(e.what());
        } catch (const std::exception& e) {
            throw SaveFileError(e.what());
        }

        DEB_INFO("Loaded game from file '{}'", SAVE_GAME_FILE);
    }

    void delete_save_game_file(const std::string& file_path) {
        if (remove(file_path.c_str()) != 0) {
            REL_INFO("Could not delete save game file '{}'", file_path.c_str());
        } else {
            REL_INFO("Deleted save game file '{}'", file_path.c_str());
        }
    }

    void handle_save_file_not_open_error() {
        bool user_data_directory;

        try {
            user_data_directory = user_data::user_data_directory_exists();
        } catch (const user_data::UserNameError& e) {
            REL_ERROR("{}", e.what());
            return;
        }

        if (!user_data_directory) {
            REL_INFO("User data folder missing; creating one...");

            try {
                bool success = user_data::create_user_data_directory();
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
