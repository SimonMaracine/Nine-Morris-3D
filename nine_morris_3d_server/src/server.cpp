#include "server.hpp"

#include <cassert>

Server::Server()
    : m_server(
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_connected(connection); },
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_disconnected(connection); }
    ) {}

void Server::start() {
    m_server.start(7915);

    using namespace std::chrono_literals;

    m_task_manager.add_delayed([this]() {
        m_server.get_logger()->debug("Collecting sessions...");

        for (auto iter {m_game_sessions.begin()}; iter != m_game_sessions.end();) {
            if (iter->second.connection1.expired() && iter->second.connection2.expired()) {
                const auto session_id {iter->first};
                iter = m_game_sessions.erase(iter);
                m_server.get_logger()->debug("Collected session {}", session_id);
            } else {
                iter++;
            }
        }

        return Task::Result::Repeat;
    }, 15s);

    m_task_manager.add_delayed([this]() {
        m_server.get_logger()->debug("Checking connections...");

        m_server.check_connections();

        return Task::Result::Repeat;
    }, 10s);
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
            case protocol::message::Client_Resign:
                client_resign(connection, message);
                break;
            case protocol::message::Client_OfferDraw:
                client_offer_draw(connection, message);
                break;
            case protocol::message::Client_AcceptDrawOffer:
                client_accept_draw_offer(connection, message);
                break;
            // case protocol::message::Client_Rematch:
            //     client_rematch(connection, message);
            //     break;
            // case protocol::message::Client_CancelRematch:
            //     client_cancel_rematch(connection, message);
            //     break;
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
    game_session.player1 = protocol::opponent(payload.remote_player_type);
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
        server_reject_join_game_session(connection, protocol::ErrorCode::SessionOccupied);
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

        payload_accept.remote_player_type = protocol::opponent(iter->second.player1);
        payload_accept.remote_player_name = iter->second.name2;

        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.expired()) {
        iter->second.connection2 = connection;
        iter->second.name2 = payload.player_name;

        payload_accept.remote_player_type = iter->second.player1;
        payload_accept.remote_player_name = iter->second.name1;

        remote_connection = iter->second.connection1.lock();
    } else {
        server_reject_join_game_session(connection, protocol::ErrorCode::SessionOccupied);
        m_server.get_logger()->warn("Client {} requested to join occupied session {}", connection->get_id(), payload.session_id);
        return;
    }

    m_clients_sessions[connection->get_id()] = iter->first;

    server_accept_join_game_session(connection, std::move(payload_accept));

    assert(remote_connection);
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
    payload.remote_player_name = remote_player_name;

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
        remote_connection = iter->second.connection2.lock();
    } else if (iter->second.connection2.lock() == connection) {
        remote_connection = iter->second.connection1.lock();
    } else {
        m_server.get_logger()->warn("Client {} played a move in session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    if (remote_connection) {
        server_remote_played_move(remote_connection, payload.move);
    }
}

void Server::server_remote_played_move(std::shared_ptr<networking::ClientConnection> connection, const std::string& move) {
    protocol::Server_RemotePlayedMove payload;
    payload.move = move;

    networking::Message message {protocol::message::Server_RemotePlayedMove};
    message.write(payload);

    m_server.send_message(connection, message);
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

void Server::client_accept_draw_offer(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
    protocol::Client_AcceptDrawOffer payload;
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
        m_server.get_logger()->warn("Client {} accepted draw offer in session {} in which it wasn't active", connection->get_id(), payload.session_id);
        return;
    }

    if (remote_connection) {
        server_remote_accepted_draw_offer(remote_connection);
    }
}

void Server::server_remote_accepted_draw_offer(std::shared_ptr<networking::ClientConnection> connection) {
    networking::Message message {protocol::message::Server_RemoteAcceptedDrawOffer};

    m_server.send_message(connection, message);
}

// void Server::client_rematch(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
//     protocol::Client_Rematch payload;
//     message.read(payload);

//     const auto iter {m_game_sessions.find(payload.session_id)};

//     if (iter == m_game_sessions.end()) {
//         m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
//         return;
//     }

//     bool rematch_both {false};

//     if (iter->second.connection1.lock() == connection) {
//         iter->second.rematch1 = true;
//         rematch_both = iter->second.rematch2;
//     } else if (iter->second.connection2.lock() == connection) {
//         iter->second.rematch2 = true;
//         rematch_both = iter->second.rematch1;
//     } else {
//         m_server.get_logger()->warn("Client {} wanted rematch in session {} in which it wasn't active", connection->get_id(), payload.session_id);
//         return;
//     }

//     if (rematch_both) {
//         // Switch sides
//         iter->second.player1 = protocol::opponent(iter->second.player1);  // FIXME didn't seem to work

//         // Restart game
//         iter->second.moves.clear();  // FIXME clearing should be done as soon as both players have game over

//         iter->second.rematch1 = false;
//         iter->second.rematch2 = false;

//         if (auto connection1 {iter->second.connection1.lock()}) {
//             server_rematch(connection1, iter->second.player1);
//         }

//         if (auto connection2 {iter->second.connection2.lock()}) {
//             server_rematch(connection2, protocol::opponent(iter->second.player1));
//         }
//     }
// }

// void Server::server_rematch(std::shared_ptr<networking::ClientConnection> connection, protocol::Player remote_player_type) {
//     protocol::Server_Rematch payload;
//     payload.remote_player_type = remote_player_type;

//     networking::Message message {protocol::message::Server_Rematch};
//     message.write(payload);

//     m_server.send_message(connection, message);
// }

// void Server::client_cancel_rematch(std::shared_ptr<networking::ClientConnection> connection, const networking::Message& message) {
//     protocol::Client_CancelRematch payload;
//     message.read(payload);

//     const auto iter {m_game_sessions.find(payload.session_id)};

//     if (iter == m_game_sessions.end()) {
//         m_server.get_logger()->warn("Session {} reported by client {} doesn't exist", connection->get_id(), payload.session_id);
//         return;
//     }

//     if (iter->second.rematch1 && iter->second.rematch2) {
//         // Reject the cancellation; both clients already agreed
//         return;
//     }

//     if (iter->second.connection1.lock() == connection) {
//         iter->second.rematch1 = false;
//     } else if (iter->second.connection2.lock() == connection) {
//         iter->second.rematch2 = false;
//     } else {
//         m_server.get_logger()->warn("Client {} wanted rematch in session {} in which it wasn't active", connection->get_id(), payload.session_id);
//         return;
//     }

//     server_cancel_rematch(connection);
// }

// void Server::server_cancel_rematch(std::shared_ptr<networking::ClientConnection> connection) {
//     networking::Message message {protocol::message::Server_CancelRematch};

//     m_server.send_message(connection, message);
// }

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
