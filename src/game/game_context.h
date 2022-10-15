#pragma once

#include "other/constants.h"

class GenericBoard;
class MinimaxThread;

class GameContext {
public:
    GameContext() = default;
    GameContext(GamePlayer white_player, GamePlayer black_player, GenericBoard* board, MinimaxThread* minimax_thread)
        : white_player(white_player), black_player(black_player), board(board), minimax_thread(minimax_thread) {}
    ~GameContext() = default;

    void begin_human_move();
    void end_human_move();
    void begin_computer_move();
    bool end_computer_move();

    void reset_player(GamePlayer player);
    bool both_players_human() { return white_player == GamePlayer::Human && black_player == GamePlayer::Human; }

    GamePlayer white_player = GamePlayer::None;
    GamePlayer black_player = GamePlayer::None;
    GameState state = GameState::MaybeNextPlayer;
private:
    GenericBoard* board = nullptr;
    MinimaxThread* minimax_thread = nullptr;
};
