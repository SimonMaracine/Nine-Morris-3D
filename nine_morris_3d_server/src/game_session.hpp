#pragma once

#include <string>
#include <vector>
#include <utility>

#include <networking/server.hpp>
#include <protocol.hpp>

struct GameSession {
    std::weak_ptr<networking::ClientConnection> connection1;
    std::weak_ptr<networking::ClientConnection> connection2;
    std::string name1;
    std::string name2;
    protocol::Moves moves;
    protocol::Messages messages;
    protocol::Player player1 {};
    protocol::ClockTime time1 {};
    protocol::ClockTime time2 {};
    protocol::ClockTime initial_time {};  // Stays constant during the whole session
    protocol::GameMode game_mode {};
    bool game_over {false};
    bool rematch1 {false};
    bool rematch2 {false};
};
