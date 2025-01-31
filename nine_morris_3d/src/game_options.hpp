#pragma once

#include "player_color.hpp"

// Data set before any game
// Represents what type of game it is and which are the players
struct GameOptions {
    int computer_color {PlayerColorBlack};
    int remote_color {PlayerColorBlack};
    int time_enum {};  // This needs to be initialized with the corresponding enum
    int custom_time {10};  // Actual time in minutes
};
