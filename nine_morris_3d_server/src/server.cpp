#include "server.hpp"

Server::Server()
    : m_server(
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_connected(connection); },
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_disconnected(connection); }
    ) {}

void Server::start() {
    m_server.start(7915);

    using namespace std::chrono_literals;
}

void Server::update() {
    m_server.accept_connections();

    while (m_server.available_messages()) {
        const auto [connection, message] {m_server.next_message()};
        handle_message(connection, message);
    }
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

void Server::server_ping(std::shared_ptr<networking::ClientConnection> connection, protocol::type::TimePoint time) {
    protocol::Server_Ping payload;
    payload.time = time;

    networking::Message message {protocol::message::Server_Ping};
    message.write(payload);

    m_server.send_message(connection, message);
}
