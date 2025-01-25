#pragma once

#include <string>
#include <vector>

#include <networking/server.hpp>
#include <protocol.hpp>

struct GameSession {
    std::shared_ptr<networking::ClientConnection> connection1;
    std::shared_ptr<networking::ClientConnection> connection2;
    std::string position;
    std::vector<std::string> moves;
    types::PlayerType player_type1;
    bool acknowledged_game_over1 {false};
    bool acknowledged_game_over2 {false};

    // TODO time-related data
};
