#pragma once

#include "nine_morris_3d/board.h"
#include "nine_morris_3d/minimax/minimax_thread.h"

enum class GamePlayer {
    None, Human, Computer
};

enum class GameState {
    MaybeNextPlayer,
    HumanBeginMove,
    HumanThinkingMove,
    HumanDoingMove,
    HumanEndMove,
    ComputerBeginMove,
    ComputerThinkingMove,
    ComputerDoingMove,
    ComputerEndMove
};

class GameContext {
public:
    GameContext() = default;
    GameContext(Board* board, MinimaxThread* minimax_thread)
        : board(board), minimax_thread(minimax_thread) {}
    ~GameContext() = default;

    void begin_human_move();
    void end_human_move();
    void begin_computer_move();
    void end_computer_move();

    void reset_player(GamePlayer player);

    GamePlayer white_player = GamePlayer::Human;  // TODO maybe remember players choices
    GamePlayer black_player = GamePlayer::Computer;
    GameState state = GameState::MaybeNextPlayer;
private:
    Board* board = nullptr;
    MinimaxThread* minimax_thread = nullptr;
};
