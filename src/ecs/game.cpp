#include <cassert>

#include <entt/entt.hpp>

#include "ecs/game.h"
#include "ecs/components.h"
#include "application/input.h"

static entt::entity put_piece(entt::registry& registry, Piece type, float x_pos, float z_pos) {
    auto view = registry.view<TransformComponent, PieceComponent>();

    for (entt::entity entity : view) {
        auto [transform, piece] = view.get<TransformComponent, PieceComponent>(entity);

        if (!piece.active && piece.type == type) {
            transform.position.x = x_pos;
            transform.position.z = z_pos;
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

        if (state.phase == Phase::PutPieces) {
            if (input::is_mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
                if (entity == hovered) {
                    const glm::vec3& position = transform.position;

                    if (state.turn == Player::White) {
                        node.piece = put_piece(registry, Piece::White,
                                               position.x, position.z);
                        state.white_pieces_count++;
                    } else {
                        node.piece = put_piece(registry, Piece::Black,
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
