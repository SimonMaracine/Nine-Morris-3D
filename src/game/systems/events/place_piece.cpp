#include <entt/entt.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/systems/events/place_piece.h"
#include "game/components/board.h"
#include "game/components/node.h"
#include "game/components/piece.h"
#include "game/board_common.h"
#include "game/constants.h"

void PlacePieceSystem::run_once(PlacePieceEvent& e) {
    bool placed = false;

    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto view = registry.view<NodeComponent, HoverComponent, TransformComponent>();

    for (auto entity : view) {
        auto [node_c, hover_c, transform_c] = view.get<NodeComponent, HoverComponent, TransformComponent>(entity);

        if (hover_c.id == e.hovered_id && entity == board_c.hovered_node && node_c.piece == entt::null) {
            remember_state(registry);
            WAIT_FOR_NEXT_MOVE();

            const glm::vec3& position = transform_c.position;

            if (board_c.turn == BoardPlayer::White) {
                node_c.piece = new_piece_to_place(registry, PieceType::White, position.x, position.z, entity);
                node_c.piece_index = registry.get<PieceComponent>(node_c.piece).index;
                board_c.white_pieces_count++;
                board_c.not_placed_pieces_count--;
            } else {
                node_c.piece = new_piece_to_place(registry, PieceType::Black, position.x, position.z, entity);
                node_c.piece_index = registry.get<PieceComponent>(node_c.piece).index;
                board_c.black_pieces_count++;
                board_c.not_placed_pieces_count--;
            }

            if (is_windmill_made(registry, entity, TURN_IS_WHITE_SO(PieceType::White, PieceType::Black))) {
                DEB_DEBUG("{} windmill is made", TURN_IS_WHITE_SO("White", "Black"));

                board_c.should_take_piece = true;
                // update_cursor_system(registry);  // TODO use app to call event

                if (board_c.turn == BoardPlayer::White) {
                    set_pieces_to_take(registry, PieceType::Black, true);
                } else {
                    set_pieces_to_take(registry, PieceType::White, true);
                }
            } else {
                check_player_number_of_pieces(registry, board_c.turn);
                switch_turn(registry);

                if (board_c.not_placed_pieces_count == 0) {
                    board_c.phase = BoardPhase::MovePieces;
                    update_outlines(registry);

                    DEB_INFO("Phase 2");

                    if (is_player_blocked(registry, board_c.turn)) {
                        DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                        FORMATTED_MESSAGE(
                            message, 64, "%s player has blocked %s player.",
                            TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("white", "black")
                        )

                        game_over(
                            registry,
                            TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                            TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                            message
                        );
                    }
                }
            }

            placed = true;
            break;
        }
    }

    if (placed) {
        // FIXME what to do?
    }
}
