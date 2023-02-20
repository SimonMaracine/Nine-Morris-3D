#pragma once

#include "other/constants.h"

struct Board;
class MinimaxThread;
struct MinimaxAlgorithm;

class GameContext {
public:
    GameContext() = default;
    GameContext(GamePlayer white_player, GamePlayer black_player, Board* board,
            MinimaxThread* minimax_thread, MinimaxAlgorithm* minimax_algorithm)
        : white_player(white_player), black_player(black_player), board(board),
          minimax_thread(minimax_thread), minimax_algorithm(minimax_algorithm) {}
    ~GameContext() = default;

    GameContext(const GameContext&) = default;
    GameContext& operator=(const GameContext&) = default;
    GameContext(GameContext&&) noexcept = default;
    GameContext& operator=(GameContext&&) noexcept = default;

    void human_begin_move();

    void computer_think_move();
    bool computer_execute_move();
    void computer_execute_take_move();

    void reset_players();
    bool both_players_human() const { return white_player == GamePlayer::Human && black_player == GamePlayer::Human; }

    GamePlayer white_player = GamePlayer::None;
    GamePlayer black_player = GamePlayer::None;
    GameState state = GameState::NextPlayer;
private:
    Board* board = nullptr;
    MinimaxThread* minimax_thread = nullptr;
    MinimaxAlgorithm* minimax_algorithm = nullptr;
};
