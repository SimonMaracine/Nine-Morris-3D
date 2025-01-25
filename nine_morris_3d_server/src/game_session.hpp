#pragma once

#include <string>
#include <vector>

#include <networking/server.hpp>

enum class PlayerType {
    White,
    Black
};

struct GameSession {
    std::shared_ptr<networking::ClientConnection> connection1;
    std::shared_ptr<networking::ClientConnection> connection2;
    std::string position;
    std::vector<std::string> moves;
    PlayerType player_type1;
    bool acknoledged_game_over1 {false};
    bool acknoledged_game_over2 {false};

    // TODO time-related data
};
