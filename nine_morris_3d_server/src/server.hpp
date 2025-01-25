#pragma once

#include <unordered_map>

#include <networking/server.hpp>

#include "game_session.hpp"
#include "periodic_task.hpp"

class Server {
public:
    Server();

    void start();
    void update();
private:
    void on_client_connected(std::shared_ptr<networking::ClientConnection> connection);
    void on_client_disconnected(std::shared_ptr<networking::ClientConnection> connection);

    void handle_message(const networking::Message& message, std::shared_ptr<networking::ClientConnection> connection);

    void server_ping();

    networking::Server m_server;

    std::unordered_map<std::uint16_t, GameSession> m_game_sessions;
    PeriodicTask m_ping_task;
};
