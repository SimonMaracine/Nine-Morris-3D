#pragma once

#include <unordered_map>

#include <networking/server.hpp>
#include <protocol.hpp>

#include "game_session.hpp"
#include "periodic_task.hpp"
#include "session_pool.hpp"

class Server {
public:
    Server();

    void start();
    void update();
private:
    void on_client_connected(std::shared_ptr<networking::ClientConnection> connection);
    void on_client_disconnected(std::shared_ptr<networking::ClientConnection> connection);

    void handle_message(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);

    void client_ping(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_ping(std::shared_ptr<networking::ClientConnection> connection, protocol::TimePoint time);
    void client_request_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_accept_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::SessionId session_id);
    void server_deny_game_session(std::shared_ptr<networking::ClientConnection> connection);
    void client_quit_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);

    networking::Server m_server;

    std::unordered_map<std::uint16_t, GameSession> m_game_sessions;
    SessionPool m_session_pool;
    PeriodicTask m_session_garbage_collector_task;
    PeriodicTask m_check_connections_task;
};
