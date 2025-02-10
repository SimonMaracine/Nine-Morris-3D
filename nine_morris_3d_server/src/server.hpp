#pragma once

#include <unordered_map>

#include <networking/server.hpp>
#include <protocol.hpp>

#include "game_session.hpp"
#include "task_manager.hpp"
#include "session_pool.hpp"
#include "configuration.hpp"

class Server {
public:
    Server(const Configuration& configuration);

    void start(const Configuration& configuration);
    void update();
private:
    void on_client_connected(std::shared_ptr<networking::ClientConnection> connection);
    void on_client_disconnected(std::shared_ptr<networking::ClientConnection> connection);

    void disconnected_client_from_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::SessionId session_id);
    void handle_message(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);

    void client_hello(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_hello_accept(std::shared_ptr<networking::ClientConnection> connection);
    void server_hello_reject(std::shared_ptr<networking::ClientConnection> connection, protocol::ErrorCode error_code);
    void client_ping(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_ping(std::shared_ptr<networking::ClientConnection> connection, protocol::TimePoint time);
    void client_request_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_accept_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::SessionId session_id);
    void server_reject_game_session(std::shared_ptr<networking::ClientConnection> connection);
    void client_request_join_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_accept_join_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::Server_AcceptJoinGameSession&& payload);
    void server_reject_join_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::ErrorCode error_code);
    void server_remote_joined_game_session(std::shared_ptr<networking::ClientConnection> connection, const std::string& remote_name);
    void client_leave_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_remote_left_game_session(std::shared_ptr<networking::ClientConnection> connection);
    void client_play_move(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_remote_played_move(std::shared_ptr<networking::ClientConnection> connection, protocol::ClockTime time, const std::string& move);
    void client_update_turn_time(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void client_resign(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_remote_resigned(std::shared_ptr<networking::ClientConnection> connection);
    void client_offer_draw(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_remote_offered_draw(std::shared_ptr<networking::ClientConnection> connection);
    void client_accept_draw(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_remote_accepted_draw(std::shared_ptr<networking::ClientConnection> connection);
    void client_send_message(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_remote_sent_message(std::shared_ptr<networking::ClientConnection> connection, const std::string& message_);
    void client_rematch(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_rematch(std::shared_ptr<networking::ClientConnection> connection, protocol::Player remote_player);
    void client_cancel_rematch(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message);
    void server_cancel_rematch(std::shared_ptr<networking::ClientConnection> connection);

    static unsigned int log_target_from_str(const std::string& string);

    networking::Server m_server;

    // Storage for the game sessions
    // Sessions are kept in memory as long as there is one client active in it
    std::unordered_map<protocol::SessionId, GameSession> m_game_sessions;

    // Map from clients to sessions
    // Should be used to quickly find out if a client is active in a session
    // Must never be out of date
    std::unordered_map<networking::ClientId, protocol::SessionId> m_clients_sessions;

    // Manager of session IDs
    SessionPool m_session_pool;

    TaskManager m_task_manager;
};
