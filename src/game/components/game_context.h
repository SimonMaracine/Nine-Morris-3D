#pragma once

#include "game/constants.h"
#include "minimax/minimax_thread.h"

struct GameContextComponent {
    GamePlayer white_player = GamePlayer::None;
    GamePlayer black_player = GamePlayer::None;
    GameState state = GameState::MaybeNextPlayer;
    entt::entity board = entt::null;
    // MinimaxThread minimax_thread;
};
