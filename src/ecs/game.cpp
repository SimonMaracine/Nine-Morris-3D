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
        auto [transform, piece, move] = view.get(entity);

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

static bool is_windmill_made(entt::registry& registry, entt::entity board,
                             entt::entity node, Piece type) {
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

            if (piece1.type == type && piece2.type == type && piece3.type == type) {
                if (piece1.node == node || piece2.node == node || piece3.node == node) {
                    return true;
                }
            }
        }
    }

    return false;
}

static void set_pieces_to_take(entt::registry& registry, Piece type, bool take) {
    auto view = registry.view<PieceComponent>();

    for (entt::entity entity : view) {
        auto& piece = view.get<PieceComponent>(entity);

        if (piece.type == type) {
            piece.to_take = take;
        }
    }
}

void systems::place_piece(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = registry.get<GameStateComponent>(board);

    auto view = registry.view<TransformComponent, NodeComponent>();

    for (entt::entity entity : view) {
        auto [transform, node] = view.get(entity);

        if (entity == hovered && entity == state.pressed_node && node.piece == entt::null) {
            const glm::vec3& position = transform.position;

            if (state.turn == Player::White) {
                node.piece = place_piece(registry, Piece::White,
                                         position.x, position.z, entity);
                state.white_pieces_count++;
                state.not_placed_pieces_count--;
            } else {
                node.piece = place_piece(registry, Piece::Black,
                                         position.x, position.z, entity);
                state.black_pieces_count++;
                state.not_placed_pieces_count--;
            }

            if (is_windmill_made(registry, board, entity,
                    state.turn == Player::White ? Piece::White : Piece::Black)) {
                SPDLOG_DEBUG("Windmill made");
                state.should_take_piece = true;

                if (state.turn == Player::White) {
                    set_pieces_to_take(registry, Piece::Black, true);
                } else {
                    set_pieces_to_take(registry, Piece::White, true);
                }
            } else {
                state.turn = switch_turn(state.turn);
            }

            if (state.not_placed_pieces_count == 0 && !state.should_take_piece) {
                state.phase = Phase::MovePieces;
                SPDLOG_INFO("Phase 2");
            }

            break;
        }
    }
}

void systems::move_piece(entt::registry& registry, float dt) {
    auto view = registry.view<TransformComponent, MoveComponent>();

    for (entt::entity entity : view) {
        auto [transform, move] = view.get(entity);

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

void systems::take_piece(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = registry.get<GameStateComponent>(board);

    auto view = registry.view<NodeComponent>();

    for (entt::entity entity : view) {
        auto& node = view.get<NodeComponent>(entity);

        if (node.piece != entt::null) {
            auto& piece = PIECE(node.piece);
            if (state.turn == Player::White) {
                if (node.piece == hovered && state.pressed_piece == hovered &&
                        piece.type == Piece::Black) {
                    if (!is_windmill_made(registry, board, entity, Piece::Black)) {
                        registry.destroy(node.piece);
                        node.piece = entt::null;
                        state.turn = switch_turn(state.turn);
                        state.should_take_piece = false;
                        set_pieces_to_take(registry, Piece::Black, false);
                        state.black_pieces_count--;
                    } else {
                        SPDLOG_DEBUG("Cannot take piece from windmill");
                    }
                }
            } else {
                if (node.piece == hovered && state.pressed_piece == hovered &&
                        piece.type == Piece::White) {
                    if (!is_windmill_made(registry, board, entity, Piece::White)) {
                        registry.destroy(node.piece);
                        node.piece = entt::null;
                        state.turn = switch_turn(state.turn);
                        state.should_take_piece = false;
                        set_pieces_to_take(registry, Piece::White, false);
                        state.white_pieces_count--;
                    } else {
                        SPDLOG_DEBUG("Cannot take piece from windmill");
                    }
                }
            }
        }
    }

    // Do this even if it may be not needed
    if (state.not_placed_pieces_count == 0 && !state.should_take_piece) {
        state.phase = Phase::MovePieces;
        SPDLOG_INFO("Phase 2");
    }
}

void systems::press(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = registry.get<GameStateComponent>(board);

    {
        auto view = registry.view<NodeComponent>();
        for (entt::entity entity : view) {
            if (entity == hovered) {
                state.pressed_node = entity;
            }
        }
    }

    {
        auto view = registry.view<PieceComponent>();
        for (entt::entity entity : view) {
            if (entity == hovered) {
                state.pressed_piece = entity;
            }
        }
    }
}

void systems::release(entt::registry& registry, entt::entity board) {
    auto& state = registry.get<GameStateComponent>(board);

    state.pressed_node = entt::null;
    state.pressed_piece = entt::null;
}
