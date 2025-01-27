#include "server.hpp"

Server::Server()
    : m_server(
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_connected(connection); },
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_disconnected(connection); }
    ) {}

void Server::start() {
    m_server.start(7915);

    using namespace std::chrono_literals;

    m_session_garbage_collector_task = PeriodicTask([this]() {
        m_server.get_logger()->debug("Collecting sessions...");

        for (auto iter {m_game_sessions.begin()}; iter != m_game_sessions.end();) {
            if (iter->second.connection1.expired() && iter->second.connection2.expired()) {
                const auto session_id {iter->first};
                iter = m_game_sessions.erase(iter);
                m_server.get_logger()->debug("Collected session with ID {}", session_id);
            } else {
                iter++;
            }
        }
    }, 12s);

    m_check_connections_task = PeriodicTask([this]() {
        m_server.get_logger()->debug("Checking connections...");

        m_server.check_connections();
    }, 10s);
}

void Server::update() {
    m_server.accept_connections();

    while (m_server.available_messages()) {
        const auto [connection, message] {m_server.next_message()};
        handle_message(connection, message);
    }

    m_session_garbage_collector_task.update();
    m_check_connections_task.update();
}

void Server::on_client_connected(std::shared_ptr<networking::ClientConnection>) {

}

void Server::on_client_disconnected(std::shared_ptr<networking::ClientConnection>) {

}

void Server::handle_message(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    try {
        switch (message.id()) {
            case protocol::message::Client_Ping:
                client_ping(connection, message);
                break;
            case protocol::message::Client_RequestGameSession:
                client_request_game_session(connection, message);
                break;
            case protocol::message::Client_QuitGameSession:
                client_quit_game_session(connection, message);
                break;
        }
    } catch (const networking::SerializationError& e) {
        m_server.get_logger()->error("Serialization error: {}", e.what());
        connection->close();
    }
}

void Server::client_ping(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_Ping payload;
    message.read(payload);

    server_ping(connection, payload.time);
}

void Server::server_ping(std::shared_ptr<networking::ClientConnection> connection, protocol::TimePoint time) {
    protocol::Server_Ping payload;
    payload.time = time;

    networking::Message message {protocol::message::Server_Ping};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::client_request_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    m_server.get_logger()->debug("Request for a new game session");

    protocol::Client_RequestGameSession payload;
    message.read(payload);

    const auto session_id {m_session_pool.alloc_session_id()};

    if (!session_id) {
        server_deny_game_session(connection);

        m_server.get_logger()->debug("Denied new session");

        return;
    }

    GameSession& game_session {m_game_sessions[*session_id]};
    game_session.connection1 = connection;
    game_session.name1 = payload.player_name;
    game_session.player1 = protocol::opponent(payload.remote_player_type);

    server_accept_game_session(connection, *session_id);

    m_server.get_logger()->debug("Created a new session with ID {}", *session_id);
}

void Server::server_accept_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::SessionId session_id) {
    protocol::Server_AcceptGameSession payload;
    payload.session_id = session_id;

    networking::Message message {protocol::message::Server_AcceptGameSession};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::server_deny_game_session(std::shared_ptr<networking::ClientConnection> connection) {
    protocol::Server_DenyGameSession payload;
    payload.error_code = protocol::ErrorCode::TooManySessions;

    networking::Message message {protocol::message::Server_DenyGameSession};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::client_quit_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_QuitGameSession payload;
    message.read(payload);

    auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.cend()) {
        return;
    }

    if (iter->second.connection1.lock() == connection) {
        iter->second.connection1.reset();
    } else if (iter->second.connection2.lock() == connection) {
        iter->second.connection2.reset();
    }
}
