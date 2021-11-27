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
// #define SAVE_ENTITIES_FILE "last_game_entities.dat"

// template<typename Archive>
// void serialize(Archive& archive, MovesHistoryComponent& moves_history) {
//     archive(moves_history.moves, moves_history.placed_pieces, moves_history.moved_pieces,
//         moves_history.taken_pieces);
// }

// template<typename Archive>
// void serialize(Archive& archive, GameStateComponent& state) {
//     archive(state.phase, state.turn, state.ending, state.white_pieces_count, state.black_pieces_count,
//         state.not_placed_pieces_count, state.should_take_piece, state.nodes, state.pressed_node,
//         state.pressed_piece, state.selected_piece, state.can_jump, state.turns_without_mills,
//         state.repetition_history);
// }

// template<typename Archive>
// void serialize(Archive& archive, NodeComponent& node) {
//     archive(node.id, node.piece);
// }

// template<typename Archive>
// void serialize(Archive& archive, PieceComponent& piece) {
//     archive(piece.id, piece.type, piece.in_use, piece.node, piece.show_outline, piece.to_take,
//         piece.pending_remove, piece.selected);
// }

// template<typename Archive>
// void serialize(Archive& archive, InactiveTag& inactive_tag) {
//     archive(inactive_tag.inactive);
// }

// template<typename Archive>
// void serialize(Archive& archive, TransformComponent& transform) {
//     archive(transform.position, transform.rotation, transform.scale);
// }

// template<typename Archive>
// void serialize(Archive& archive, CameraComponent& camera) {
//     archive(camera.point, camera.distance_to_point, camera.angle_around_point);
// }

// namespace undo {
//     template<typename Archive>
//     void serialize(Archive& archive, PlacedPiece& placed_piece) {
//         archive(placed_piece.state, placed_piece.nodes, placed_piece.pieces, placed_piece.transforms);
//     }

//     template<typename Archive>
//     void serialize(Archive& archive, MovedPiece& moved_piece) {
//         archive(moved_piece.state, moved_piece.nodes, moved_piece.pieces, moved_piece.transforms);
//     }

//     template<typename Archive>
//     void serialize(Archive& archive, TakenPiece& taken_piece) {
//         archive(taken_piece.state, taken_piece.nodes, taken_piece.pieces, taken_piece.removed_piece,
//             taken_piece.removed_piece_entity, taken_piece.transform);
//     }
// }

// template<typename Archive>
// void serialize(Archive& archive, ThreefoldRepetitionHistory& repetition_history) {
//     archive(repetition_history.ones, repetition_history.twos);
// }

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

    // Entities gather_entities(entt::entity board, entt::entity camera, entt::entity* nodes, entt::entity* pieces) {
    //     Entities entities;

    //     entities.board = board;
    //     entities.camera = camera;
    //     for (int i = 0; i < 24; i++) {
    //         entities.nodes[i] = nodes[i];
    //     }
    //     for (int i = 0; i < 18; i++) {
    //         entities.pieces[i] = pieces[i];
    //     }

    //     return entities;
    // }

    // void reset_entities(const Entities& entities, entt::entity* board, entt::entity* camera, entt::entity* nodes, entt::entity* pieces) {
    //     *board = entities.board;
    //     *camera = entities.camera;
    //     for (int i = 0; i < 24; i++) {
    //         nodes[i] = entities.nodes[i];
    //     }
    //     for (int i = 0; i < 18; i++) {
    //         pieces[i] = entities.pieces[i];
    //     }
    // }

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
