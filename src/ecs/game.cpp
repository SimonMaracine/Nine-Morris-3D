#include <cassert>

#include <entt/entt.hpp>

#include "ecs/game.h"
#include "ecs/components.h"
#include "application/input.h"
#include "other/logging.h"

static entt::entity place_piece(entt::registry& registry, Piece type, float x_pos, float z_pos) {
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

void game_update_system(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = registry.get<GameStateComponent>(board);

    auto view = registry.view<TransformComponent, NodeComponent>();

    for (entt::entity entity : view) {
        auto [transform, node] = view.get<TransformComponent, NodeComponent>(entity);

        if (state.phase == Phase::PlacePieces) {
            if (input::is_mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
                if (entity == hovered && node.piece == entt::null) {
                    const glm::vec3& position = transform.position;

                    if (state.turn == Player::White) {
                        node.piece = place_piece(registry, Piece::White,
                                                 position.x, position.z);
                        state.white_pieces_count++;
                    } else {
                        node.piece = place_piece(registry, Piece::Black,
                                                 position.x, position.z);
                        state.black_pieces_count++;
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

void piece_move_system(entt::registry& registry, float dt) {
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
