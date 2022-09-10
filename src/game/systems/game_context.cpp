// #include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "game/components/board.h"
// #include "game/components/game_context.h"
// #include "game/systems/game_context.h"
// #include "minimax/main_game/minimax_main_game.h"
// // #include "other/assert.h"

// void begin_human_move_system(entt::registry& registry) {
//     auto view = registry.view<BoardComponent>();
//     auto& board_c = view.get<BoardComponent>(view.back());

//     board_c.is_players_turn = true;
// }

// void end_human_move_system(entt::registry& registry) {
//     auto view = registry.view<BoardComponent>();
//     auto& board_c = view.get<BoardComponent>(view.back());

//     board_c.is_players_turn = false;
// }

// void begin_computer_move_system(entt::registry& registry) {
//     // TODO start the appropriate minimax algorithm in thread

//     auto board_view = registry.view<BoardComponent>();
//     auto& board_c = board_view.get<BoardComponent>(board_view.back());

//     if (board_c.should_take_piece) {
//         return;  // Do nothing in this case
//     }

//     auto view = registry.view<GameContextComponent>();
//     auto& game_context_c = view.get<GameContextComponent>(view.back());

//     game_context_c.minimax_thread.start(minimax_main_game::minimax);
// }

// void end_computer_move_system(entt::registry& registry) {
//     auto board_view = registry.view<BoardComponent>();
//     auto& board_c = board_view.get<BoardComponent>(board_view.back());

//     if (board_c.phase == BoardPhase::PlacePieces) {
//         if (board_c.should_take_piece) {
//             // board->computer_take_piece(minimax_thread->get_result().take_node_index);
//         } else {
//             // board->computer_place_piece(minimax_thread->get_result().place_node_index);
//         }
//     } else if (board_c.phase == BoardPhase::MovePieces) {
//         if (board_c.should_take_piece) {
//             // board->computer_take_piece(minimax_thread->get_result().take_node_index);
//         } else {
//             const MinimaxThread::Result& result = minimax_thread->get_result();
//             // board->computer_put_down_piece(result.put_down_source_node_index, result.put_down_destination_node_index);
//         }
//     }
// }

// void reset_player_system(entt::registry& registry, GamePlayer player) {
//     auto board_view = registry.view<BoardComponent>();
//     auto& board_c = board_view.get<BoardComponent>(board_view.back());

//     auto view = registry.view<GameContextComponent>();
//     auto& game_context_c = view.get<GameContextComponent>(view.back());

//     switch (player) {
//         case GamePlayer::None:
//             ASSERT(false, "Player must not be None");
//             break;
//         case GamePlayer::Human:
//             board_c.is_players_turn = false;
//             break;
//         case GamePlayer::Computer:
//             break;  // Nothing needs to be done
//     }

//     game_context_c.state = GameState::MaybeNextPlayer;
// }
