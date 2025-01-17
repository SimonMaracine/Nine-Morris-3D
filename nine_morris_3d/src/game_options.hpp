#pragma once

#include "constants.hpp"

// Data set before any game

struct GameOptions {
    template<typename T>
    explicit GameOptions(T time)
        : time(static_cast<int>(time)) {}

    int game_type {static_cast<int>(GameType::Local)};  // FIXME use a better model
    int white_player {static_cast<int>(GamePlayer::Human)};
    int black_player {static_cast<int>(GamePlayer::Human)};
    int time {};
};
