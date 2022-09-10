#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/systems/events/take_piece.h"
#include "game/components/board.h"
#include "game/components/piece.h"
#include "game/components/active.h"
#include "game/components/node.h"
#include "game/board_common.h"

void TakePieceSystem::run_once(TakePieceEvent& e) {
    bool took = false;

    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto view = registry.view<PieceComponent, HoverComponent, TransformComponent, ActiveComponent>();

    if (board_c.hovered_piece != entt::null) {  // Do anything only if there is a hovered piece
        for (auto entity : view) {
            auto [piece_c, hover_c, transform_c] = view.get<PieceComponent, HoverComponent, TransformComponent>(entity);

            if (board_c.turn == BoardPlayer::White) {
                if (hover_c.id == e.hovered_id && piece_c.type == PieceType::Black) {
                    ASSERT(registry.get<HoverComponent>(board_c.hovered_piece).id == e.hovered_id, "They must be equal");

                    if (!is_windmill_made(registry, piece_c.node, PieceType::Black)
                            || number_of_pieces_in_windmills(registry, PieceType::Black) == board_c.black_pieces_count) {
                        ASSERT(piece_c.in_use, "Piece must be in use");

                        remember_state(registry);
                        WAIT_FOR_NEXT_MOVE();

                        auto& node_c = registry.get<NodeComponent>(piece_c.node);

                        node_c.piece = entt::null;
                        node_c.piece_index = INVALID_PIECE_INDEX;
                        take_and_raise_piece(registry, entity);
                        board_c.should_take_piece = false;
                        // update_cursor_system(registry);  // TODO don't call other systems
                        // TODO use app to call event
                        set_pieces_to_take(registry, PieceType::Black, false);
                        board_c.black_pieces_count--;
                        check_player_number_of_pieces(registry, BoardPlayer::Black);
                        check_player_number_of_pieces(registry, BoardPlayer::White);
                        switch_turn(registry);
                        update_outlines(registry);

                        DEB_DEBUG("Black piece {} taken", e.hovered_id);

                        if (is_player_blocked(registry, board_c.turn)) {
                            DEB_INFO("{} player is blocked", TURN_IS_WHITE_SO("White", "Black"));

                            FORMATTED_MESSAGE(
                                message, 64, "%s player has blocked %s player.",
                                TURN_IS_WHITE_SO("Black", "White"), TURN_IS_WHITE_SO("White", "Black")
                            )

                            game_over(
                                registry,
                                TURN_IS_WHITE_SO(BoardEnding::WinnerBlack, BoardEnding::WinnerWhite),
                                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                                message
                            );
                        }
                    } else {
                        DEB_DEBUG("Cannot take piece from windmill");
                    }

                    took = true;
                    break;
                }
            } else {
                if (hover_c.id == e.hovered_id && piece_c.type == PieceType::White) {
                    ASSERT(registry.get<HoverComponent>(board_c.hovered_piece).id == e.hovered_id, "They must be equal");

                    if (!is_windmill_made(registry, piece_c.node, PieceType::White)
                            || number_of_pieces_in_windmills(registry, PieceType::White) == board_c.white_pieces_count) {
                        ASSERT(piece_c.in_use, "Piece must be in use");

                        remember_state(registry);
                        WAIT_FOR_NEXT_MOVE();

                        auto& node_c = registry.get<NodeComponent>(piece_c.node);

                        node_c.piece = entt::null;
                        node_c.piece_index = INVALID_PIECE_INDEX;
                        take_and_raise_piece(registry, entity);
                        board_c.should_take_piece = false;
                        // update_cursor_system(registry);  // TODO use app to call event
                        set_pieces_to_take(registry, PieceType::White, false);
                        board_c.white_pieces_count--;
                        check_player_number_of_pieces(registry, BoardPlayer::Black);
                        check_player_number_of_pieces(registry, BoardPlayer::White);
                        switch_turn(registry);
                        update_outlines(registry);

                        DEB_DEBUG("White piece {} taken", e.hovered_id);

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
                    } else {
                        DEB_DEBUG("Cannot take piece from windmill");
                    }

                    took = true;
                    break;
                }
            }
        }

        // Do this even if it may not be needed
        if (board_c.not_placed_pieces_count == 0 && !board_c.should_take_piece && board_c.phase != BoardPhase::GameOver) {
            board_c.phase = BoardPhase::MovePieces;
            update_outlines(registry);

            DEB_INFO("Phase 2");
        }
    }

    if (took) {
        // FIXME what to do?
    }
}
