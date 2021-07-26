#include <cassert>
// #include <algorithm>

#include <entt/entt.hpp>

#include "ecs/game.h"
#include "ecs/components.h"
#include "application/input.h"
#include "other/logging.h"

static entt::entity place_piece(entt::registry& registry, Piece type, float x_pos,
                                float z_pos, entt::entity node_entity) {
    auto view = registry.view<TransformComponent, PieceComponent, MoveComponent>();

    for (entt::entity entity : view) {
        auto [transform, piece, move] = view.get<TransformComponent, PieceComponent,
                                                 MoveComponent>(entity);

        if (!piece.active && piece.type == type) {
            move.target.x = x_pos;
            move.target.y = PIECE_Y_POSITION;
            move.target.z = z_pos;

            move.velocity = (move.target - transform.position) * PIECE_MOVE_SPEED;
            move.should_move = true;

            piece.active = true;
            piece.node = node_entity;

            return entity;
        }
    }

    assert(false);
}

static Player switch_turn(Player turn) {
    if (turn == Player::White) {
        return Player::Black;
    } else {
        return Player::White;
    }
}

static bool is_windmill_made(entt::registry& registry, entt::entity board, entt::entity node) {
    constexpr int windmills[16][3] = {
        { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
        { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
        { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
        { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
    };

    auto& state = registry.get<GameStateComponent>(board);

    for (int i = 0; i < 16; i++) {
        const int* mill = windmills[i];

        auto& node1 = NODE(state.nodes[mill[0]]);
        auto& node2 = NODE(state.nodes[mill[1]]);
        auto& node3 = NODE(state.nodes[mill[2]]);

        if (node1.piece != entt::null && node2.piece != entt::null &&
                node3.piece != entt::null) {
            auto& piece1 = PIECE(node1.piece);
            auto& piece2 = PIECE(node2.piece);
            auto& piece3 = PIECE(node3.piece);

            if (piece1.type == piece2.type && piece2.type == piece3.type) {
                if (piece1.node == node || piece2.node == node || piece3.node == node) {
                    return true;
                }
            }
        }
    }

    return false;
}

void systems::game_update(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = registry.get<GameStateComponent>(board);

    auto view = registry.view<TransformComponent, NodeComponent>();

    for (entt::entity entity : view) {
        auto [transform, node] = view.get<TransformComponent, NodeComponent>(entity);

        if (state.phase == Phase::PlacePieces) {
            if (input::is_mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
                if (entity == hovered && node.piece == entt::null) {
                    SPDLOG_DEBUG("Placing piece");
                    const glm::vec3& position = transform.position;

                    if (state.turn == Player::White) {
                        node.piece = place_piece(registry, Piece::White,
                                                 position.x, position.z, entity);
                        state.white_pieces_count++;
                    } else {
                        node.piece = place_piece(registry, Piece::Black,
                                                 position.x, position.z, entity);
                        state.black_pieces_count++;
                    }

                    if (is_windmill_made(registry, board, entity)) {
                        SPDLOG_DEBUG("WINDMILL MADE");
                        
                    }

                    state.turn = switch_turn(state.turn);

                    if (state.white_pieces_count == 9 && state.black_pieces_count == 9) {
                        state.phase = Phase::MovePieces;
                    }
                }
            }
        } else if (state.phase == Phase::MovePieces) {

        } else {

        }
    }
}

void systems::piece_move(entt::registry& registry, float dt) {
    auto view = registry.view<TransformComponent, MoveComponent>();

    for (entt::entity entity : view) {
        auto [transform, move] = view.get<TransformComponent, MoveComponent>(entity);

        if (move.should_move) {
            if (glm::length(move.target - transform.position) > 0.35f) {
                transform.position += move.velocity * dt;
            } else if (glm::length(move.target - transform.position) > 0.1f) {
                transform.position += move.velocity * 0.5f * dt;
            } else {
                transform.position = move.target;

                // Reset all
                move.should_move = false;
                move.velocity = glm::vec3(0.0f);
                move.target = glm::vec3(0.0f);
            }
        }
    }
}
