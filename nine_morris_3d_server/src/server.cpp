#include "server.hpp"

static constexpr unsigned int VERSION_MAJOR {0};
static constexpr unsigned int VERSION_MINOR {2};
static constexpr unsigned int VERSION_PATCH {0};

Server::Server(const Configuration& configuration)
    : m_server(
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_connected(connection); },
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_disconnected(connection); },
        log_target_from_str(configuration.log_target)
    ) {}

void Server::start(const Configuration& configuration) {
    m_server.get_logger()->set_level(spdlog::level::from_str(configuration.log_level));

    m_server.get_logger()->info("Server version {}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    m_server.start(configuration.port, configuration.max_clients);

    using namespace std::chrono_literals;

    m_task_manager.add_delayed([this]() {
        m_server.get_logger()->debug("Collecting sessions...");

        for (auto iter {m_game_sessions.begin()}; iter != m_game_sessions.end();) {
            if (iter->second.connection1.expired() && iter->second.connection2.expired()) {
                const auto session_id {iter->first};
                iter = m_game_sessions.erase(iter);
                m_session_pool.free_session_id(session_id);
                m_server.get_logger()->debug("Collected session {}", session_id);
            } else {
                iter++;
            }
        }

        return Task::Result::Repeat;
    }, configuration.session_collect_period);

    m_task_manager.add_delayed([this]() {
        m_server.get_logger()->debug("Checking connections...");

        m_server.check_connections();

        return Task::Result::Repeat;
    }, configuration.connection_check_period);
}

void Server::update() {
    m_server.accept_connections();

    while (m_server.available_messages()) {
        const auto [connection, message] {m_server.next_message()};
        handle_message(connection, message);
    }

    m_task_manager.update();
}

void Server::on_client_connected(std::shared_ptr<networking::ClientConnection>) {

}

void Server::on_client_disconnected(std::shared_ptr<networking::ClientConnection> connection) {
    const auto iter {m_clients_sessions.find(connection->get_id())};

    if (iter == m_clients_sessions.end()) {
        return;
    }

    disconnected_client_from_game_session(connection, iter->second);

    m_clients_sessions.erase(iter);
}

void Server::disconnected_client_from_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::SessionId session_id) {
    const auto iter {m_game_sessions.find(session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;

    if (iter->second.connection1.lock() == connection) {
        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.lock() == connection) {
        remote_connection = iter->second.connection1.lock();
    } else {
        m_server.get_logger()->warn("Client {} not active in session {}", connection->get_id(), session_id);
    }

    if (remote_connection) {
        // Cannot send messages inside on_client_disconnected
        m_task_manager.add_immediate([this, remote_connection]() {
            server_remote_left_game_session(remote_connection);

            return Task::Result::Done;
        });
    }
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
            case protocol::message::Client_RequestJoinGameSession:
                client_request_join_game_session(connection, message);
                break;
            case protocol::message::Client_LeaveGameSession:
                client_leave_game_session(connection, message);
                break;
            case protocol::message::Client_PlayMove:
                client_play_move(connection, message);
                break;
            case protocol::message::Client_UpdateTurnTime:
                client_update_turn_time(connection, message);
                break;
            case protocol::message::Client_Resign:
                client_resign(connection, message);
                break;
            case protocol::message::Client_OfferDraw:
                client_offer_draw(connection, message);
                break;
            case protocol::message::Client_AcceptDraw:
                client_accept_draw(connection, message);
                break;
            case protocol::message::Client_SendMessage:
                client_send_message(connection, message);
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
    m_server.get_logger()->debug("Request for a new game session from client {}", connection->get_id());

    protocol::Client_RequestGameSession payload;
    message.read(payload);

    const auto session_id {m_session_pool.alloc_session_id()};

    if (!session_id) {
        server_reject_game_session(connection);
        m_server.get_logger()->debug("Rejected new session to client {}", connection->get_id());
        return;
    }

    GameSession& game_session {m_game_sessions[*session_id]};
    game_session.connection1 = connection;
    game_session.name1 = payload.player_name;
    game_session.player1 = protocol::opponent(payload.remote_player);
    game_session.time1 = payload.time;
    game_session.time2 = payload.time;
    game_session.game_mode = payload.game_mode;

    m_clients_sessions[connection->get_id()] = *session_id;

    server_accept_game_session(connection, *session_id);

    m_server.get_logger()->debug("Created new session {} for client {}", *session_id, connection->get_id());
}

void Server::server_accept_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::SessionId session_id) {
    protocol::Server_AcceptGameSession payload;
    payload.session_id = session_id;

    networking::Message message {protocol::message::Server_AcceptGameSession};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::server_reject_game_session(std::shared_ptr<networking::ClientConnection> connection) {
    protocol::Server_RejectGameSession payload;
    payload.error_code = protocol::ErrorCode::TooManySessions;

    networking::Message message {protocol::message::Server_RejectGameSession};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::client_request_join_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_RequestJoinGameSession payload;
    message.read(payload);

    const auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        server_reject_join_game_session(connection, protocol::ErrorCode::InvalidSessionId);
        m_server.get_logger()->debug("Client {} requested to join and invalid session", connection->get_id());
        return;
    }

    if (iter->second.connection1.expired() && iter->second.connection2.expired()) {
        server_reject_join_game_session(connection, protocol::ErrorCode::SessionExpired);
        m_server.get_logger()->warn("Client {} requested to join empty session {}", connection->get_id(), payload.session_id);
        return;
    }

    if (iter->second.game_mode != payload.game_mode) {
        server_reject_join_game_session(connection, protocol::ErrorCode::SessionDifferentGame);
        m_server.get_logger()->debug("Client {} requested to join session {} with a different game", connection->get_id(), payload.session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;

    protocol::Server_AcceptJoinGameSession payload_accept;
    payload_accept.session_id = payload.session_id;
    payload_accept.moves = iter->second.moves;
    payload_accept.messages = iter->second.messages;

    if (iter->second.connection1.expired()) {
        iter->second.connection1 = connection;
        iter->second.name1 = payload.player_name;

        payload_accept.remote_player = protocol::opponent(iter->second.player1);
        payload_accept.remote_time = iter->second.time2;
        payload_accept.time = iter->second.time1;
        payload_accept.remote_name = iter->second.name2;

        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.expired()) {
        iter->second.connection2 = connection;
        iter->second.name2 = payload.player_name;

        payload_accept.remote_player = iter->second.player1;
        payload_accept.remote_time = iter->second.time1;
        payload_accept.time = iter->second.time2;
        payload_accept.remote_name = iter->second.name1;

        remote_connection = iter->second.connection1.lock();
    } else {
        server_reject_join_game_session(connection, protocol::ErrorCode::SessionOccupied);
        m_server.get_logger()->warn("Client {} requested to join occupied session {}", connection->get_id(), payload.session_id);
        return;
    }

    m_clients_sessions[connection->get_id()] = iter->first;

    server_accept_join_game_session(connection, std::move(payload_accept));

    server_remote_joined_game_session(remote_connection, payload.player_name);
}

void Server::server_accept_join_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::Server_AcceptJoinGameSession&& payload) {
    networking::Message message {protocol::message::Server_AcceptJoinGameSession};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::server_reject_join_game_session(std::shared_ptr<networking::ClientConnection> connection, protocol::ErrorCode error_code) {
    protocol::Server_RejectJoinGameSession payload;
    payload.error_code = error_code;

    networking::Message message {protocol::message::Server_RejectJoinGameSession};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::server_remote_joined_game_session(std::shared_ptr<networking::ClientConnection> connection, const std::string& remote_player_name) {
    protocol::Server_RemoteJoinedGameSession payload;
    payload.remote_name = remote_player_name;

    networking::Message message {protocol::message::Server_RemoteJoinedGameSession};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::client_leave_game_session(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_LeaveGameSession payload;
    message.read(payload);

    auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;

    if (iter->second.connection1.lock() == connection) {
        iter->second.connection1.reset();
        m_clients_sessions.erase(connection->get_id());
        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.lock() == connection) {
        iter->second.connection2.reset();
        m_clients_sessions.erase(connection->get_id());
        remote_connection = iter->second.connection1.lock();
    } else {
        m_server.get_logger()->warn("Client {} left session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    if (remote_connection) {
        server_remote_left_game_session(remote_connection);
    }
}

void Server::server_remote_left_game_session(std::shared_ptr<networking::ClientConnection> connection) {
    networking::Message message {protocol::message::Server_RemoteLeaveGameSession};

    m_server.send_message(connection, message);
}

void Server::client_play_move(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_PlayMove payload;
    message.read(payload);

    const auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;

    iter->second.moves.push_back(payload.move);

    if (iter->second.connection1.lock() == connection) {
        iter->second.time1 = payload.time;
        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.lock() == connection) {
        iter->second.time2 = payload.time;
        remote_connection = iter->second.connection1.lock();
    } else {
        m_server.get_logger()->warn("Client {} played a move in session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    if (remote_connection) {
        server_remote_played_move(remote_connection, payload.time, payload.move);
    }
}

void Server::server_remote_played_move(std::shared_ptr<networking::ClientConnection> connection, protocol::ClockTime time, const std::string& move) {
    protocol::Server_RemotePlayedMove payload;
    payload.time = time;
    payload.move = move;

    networking::Message message {protocol::message::Server_RemotePlayedMove};
    message.write(payload);

    m_server.send_message(connection, message);
}

void Server::client_update_turn_time(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_UpdateTurnTime payload;
    message.read(payload);

    const auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
        return;
    }

    if (iter->second.connection1.lock() == connection) {
        iter->second.time1 = payload.time;
    } else if (iter->second.connection2.lock() == connection) {
        iter->second.time2 = payload.time;
    } else {
        m_server.get_logger()->warn("Client {} updated time in session {} in which it wasn't active", connection->get_id(), payload.session_id);
    }
}

void Server::client_resign(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_Resign payload;
    message.read(payload);

    const auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;

    if (iter->second.connection1.lock() == connection) {
        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.lock() == connection) {
        remote_connection = iter->second.connection1.lock();
    } else {
        m_server.get_logger()->warn("Client {} resigned in session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    if (remote_connection) {
        server_remote_resigned(remote_connection);
    }
}

void Server::server_remote_resigned(std::shared_ptr<networking::ClientConnection> connection) {
    networking::Message message {protocol::message::Server_RemoteResigned};

    m_server.send_message(connection, message);
}

void Server::client_offer_draw(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_OfferDraw payload;
    message.read(payload);

    const auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;

    if (iter->second.connection1.lock() == connection) {
        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.lock() == connection) {
        remote_connection = iter->second.connection1.lock();
    } else {
        m_server.get_logger()->warn("Client {} offered draw in session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    if (remote_connection) {
        server_remote_offered_draw(remote_connection);
    }
}

void Server::server_remote_offered_draw(std::shared_ptr<networking::ClientConnection> connection) {
    networking::Message message {protocol::message::Server_RemoteOfferedDraw};

    m_server.send_message(connection, message);
}

void Server::client_accept_draw(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_AcceptDraw payload;
    message.read(payload);

    const auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;

    if (iter->second.connection1.lock() == connection) {
        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.lock() == connection) {
        remote_connection = iter->second.connection1.lock();
    } else {
        m_server.get_logger()->warn("Client {} accepted draw in session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    if (remote_connection) {
        server_remote_accepted_draw(remote_connection);
    }
}

void Server::server_remote_accepted_draw(std::shared_ptr<networking::ClientConnection> connection) {
    networking::Message message {protocol::message::Server_RemoteAcceptedDraw};

    m_server.send_message(connection, message);
}

void Server::client_send_message(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_SendMessage payload;
    message.read(payload);

    const auto iter {m_game_sessions.find(payload.session_id)};

    if (iter == m_game_sessions.end()) {
        m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
        return;
    }

    std::shared_ptr<networking::ClientConnection> remote_connection;
    std::string_view name;

    if (iter->second.connection1.lock() == connection) {
        remote_connection = iter->second.connection2.lock();
        name = iter->second.name1;
    } else if (iter->second.connection2.lock() == connection) {
        remote_connection = iter->second.connection1.lock();
        name = iter->second.name2;
    } else {
        m_server.get_logger()->warn("Client {} sent message in session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    iter->second.messages.emplace_back(name, payload.message);

    if (remote_connection) {
        server_remote_sent_message(remote_connection, payload.message);
    }
}

void Server::server_remote_sent_message(std::shared_ptr<networking::ClientConnection> connection, const std::string& message_) {
    protocol::Server_RemoteSentMessage payload;
    payload.message = message_;

    networking::Message message {protocol::message::Server_RemoteSentMessage};
    message.write(payload);

    m_server.send_message(connection, message);
}

unsigned int Server::log_target_from_str(const std::string& string) {
    if (string == "none") {
        return networking::LogTarget::LogTargetNone;
    } else if (string == "console") {
        return networking::LogTarget::LogTargetConsole;
    } else if (string == "file") {
        return networking::LogTarget::LogTargetFile;
    } else if (string == "both") {
        return networking::LogTarget::LogTargetConsole | networking::LogTarget::LogTargetFile;
    }

    return networking::LogTarget::LogTargetNone;
}
