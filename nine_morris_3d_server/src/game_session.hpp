#pragma once

#include <string>
#include <vector>

#include <networking/server.hpp>
#include <protocol.hpp>

struct GameSession {
    std::weak_ptr<networking::ClientConnection> connection1;
    std::weak_ptr<networking::ClientConnection> connection2;
    std::string name1;
    std::string name2;
    std::vector<std::string> moves;
    protocol::Messages messages;
    protocol::Player player1 {};
    protocol::ClockTime time1 {0};
    protocol::ClockTime time2 {0};
    protocol::GameMode game_mode {};
    bool game_over {false};
    bool rematch1 {false};
    bool rematch2 {false};
};
