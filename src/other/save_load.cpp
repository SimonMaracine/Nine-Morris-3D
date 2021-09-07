#include <fstream>

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>

#include "other/save_load.h"
#include "ecs_and_game/components.h"
#include "ecs_and_game/game.h"
#include "other/logging.h"

#define SAVE_GAME_FILE "last_game.dat"

using json = nlohmann::json;

template<typename Archive>
void serialize(Archive& archive, MovesHistoryComponent& moves_history) {
    archive(moves_history.moves, moves_history.placed_pieces);
}

template<typename Archive>
void serialize(Archive& archive, GameStateComponent& state) {
    archive(state.phase, state.turn, state.ending, state.white_pieces_count, state.black_pieces_count,
        state.not_placed_pieces_count, state.should_take_piece, state.nodes, state.pressed_node,
        state.pressed_piece, state.selected_piece, state.can_jump, state.turns_without_mills,
        state.repetition_history);
}

template<typename Archive>
void serialize(Archive& archive, NodeComponent& node) {
    archive(node.id, node.piece);
}

template<typename Archive>
void serialize(Archive& archive, PieceComponent& piece) {
    archive(piece.id, piece.type, piece.in_use, piece.node, piece.show_outline, piece.to_take,
        piece.pending_remove, piece.selected);
}

template<typename Archive>
void serialize(Archive& archive, InactiveTag& inactive_tag) {
    archive(inactive_tag.inactive);
}

template<typename Archive>
void serialize(Archive& archive, TransformComponent& transform) {
    archive(transform.position, transform.rotation, transform.scale);
}

template<typename Archive>
void serialize(Archive& archive, CameraComponent& camera) {
    archive(camera.view_matrix, camera.projection_matrix, camera.projection_view_matrix,
        camera.point, camera.distance_to_point, camera.angle_around_point);
}

namespace undo {
    template<typename Archive>
    void serialize(Archive& archive, PlacedPiece& placed_piece) {
        archive(placed_piece.state, placed_piece.nodes, placed_piece.pieces, placed_piece.transforms);
    }

    template<typename Archive>
    void serialize(Archive& archive, MovedPiece& moved_piece) {
        archive(moved_piece.state, moved_piece.nodes, moved_piece.pieces, moved_piece.transforms);
    }

    template<typename Archive>
    void serialize(Archive& archive, TakenPiece& taken_piece) {
        archive(taken_piece.state, taken_piece.nodes, taken_piece.pieces, taken_piece.removed_piece,
            taken_piece.removed_piece_entity, taken_piece.transform);
    }
}

template<typename Archive>
void serialize(Archive& archive, ThreefoldRepetitionHistory& repetition_history) {
    archive(repetition_history.ones, repetition_history.twos);
}

namespace glm {
    template<typename Archive>
    void serialize(Archive& archive, vec3& vector) {
        archive(vector.x, vector.y, vector.z);
    }

    template<typename Archive>
    void serialize(Archive& archive, glm::mat4& matrix) {
        archive(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
            matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
            matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
            matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
    }
}

namespace save_load {
    void save_game(const entt::registry& registry) {
        std::ofstream file = std::ofstream(SAVE_GAME_FILE, std::ios::out | std::ios::binary | std::ios::trunc);

        cereal::BinaryOutputArchive output{file};
        entt::snapshot{registry}.entities(output).component<MovesHistoryComponent,
            GameStateComponent, NodeComponent, PieceComponent, InactiveTag,
            TransformComponent, CameraComponent>(output);

        file.close();

        SPDLOG_INFO("Saved game to file '{}'", SAVE_GAME_FILE);
    }

    void load_game(entt::registry& registry) {
        std::ifstream file = std::ifstream(SAVE_GAME_FILE, std::ios::in | std::ios::binary);

        if (!file.is_open()) {
            spdlog::error("Could not open the save game file '{}'", SAVE_GAME_FILE);
            return;
        }

        cereal::BinaryInputArchive input{file};
        entt::snapshot_loader{registry}.entities(input).component<MovesHistoryComponent,
            GameStateComponent, NodeComponent, PieceComponent, InactiveTag,
            TransformComponent, CameraComponent>(input).orphans();

        file.close();

        SPDLOG_INFO("Loaded game from file '{}'", SAVE_GAME_FILE);
    }
}
