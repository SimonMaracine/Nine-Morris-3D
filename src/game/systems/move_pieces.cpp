#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/components/piece.h"
#include "game/components/piece_move.h"
#include "game/components/active.h"
#include "game/systems/move_pieces.h"
#include "game/board_common.h"
#include "game/constants.h"

void MovePiecesSystem::run() {
    constexpr float LIMIT = 0.03f;

    auto view = registry.view<PieceComponent, PieceMoveComponent, TransformComponent, ActiveComponent>();

    for (auto entity : view) {
        auto [piece_c, piece_move_c, transform_c] = view.get<PieceComponent, PieceMoveComponent, TransformComponent>(entity);

        if (!piece_move_c.moving) {
            continue;
        }

        switch (piece_move_c.type) {
            case PieceMovementType::None:
                ASSERT(false, "Movement type None is invalid");

                break;
            case PieceMovementType::Linear:
                transform_c.position += piece_move_c.velocity * app->get_delta() + (piece_move_c.target - transform_c.position)
                        * PIECE_VARIABLE_VELOCITY * app->get_delta();

                if (glm::length(piece_move_c.target - transform_c.position) < LIMIT) {
                    arrive_at_node(registry, entity);
                }

                break;
            case PieceMovementType::ThreeStep:
                if (!piece_move_c.reached_target0) {
                    transform_c.position += piece_move_c.velocity * app->get_delta() + (piece_move_c.target0 - transform_c.position)
                            * PIECE_VARIABLE_VELOCITY * app->get_delta();
                } else if (!piece_move_c.reached_target1) {
                    transform_c.position += piece_move_c.velocity * app->get_delta() + (piece_move_c.target1 - transform_c.position)
                            * PIECE_VARIABLE_VELOCITY * app->get_delta();
                } else {
                    transform_c.position += piece_move_c.velocity * app->get_delta() + (piece_move_c.target - transform_c.position)
                            * PIECE_VARIABLE_VELOCITY * app->get_delta();
                }

                if (!piece_move_c.reached_target0 && glm::length(piece_move_c.target0 - transform_c.position) < LIMIT) {
                    piece_move_c.reached_target0 = true;
                    transform_c.position = piece_move_c.target0;
                    piece_move_c.velocity = glm::normalize(piece_move_c.target1 - transform_c.position)
                            * PIECE_BASE_VELOCITY;
                } else if (!piece_move_c.reached_target1 && glm::length(piece_move_c.target1 - transform_c.position) < LIMIT) {
                    piece_move_c.reached_target1 = true;
                    transform_c.position = piece_move_c.target1;
                    piece_move_c.velocity = glm::normalize(piece_move_c.target - transform_c.position)
                            * PIECE_BASE_VELOCITY;
                }

                if (glm::length(piece_move_c.target - transform_c.position) < LIMIT) {
                    arrive_at_node(registry, entity);
                }

                break;
        }
    }
}
