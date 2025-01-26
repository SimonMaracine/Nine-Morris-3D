#include "server.hpp"

#include <protocol.hpp>

Server::Server()
    : m_server(
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_connected(connection); },
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_disconnected(connection); }
    ) {}

void Server::start() {
    m_server.start(7915);

    using namespace std::chrono_literals;

    m_ping_task = PeriodicTask([this]() {
        server_ping();
    }, 10s);
}

void Server::update() {
    m_server.accept_connections();

    while (m_server.available_messages()) {
        const auto [message, connection] {m_server.next_message()};
        handle_message(message, connection);
    }

    m_ping_task.update();
}

void Server::on_client_connected(std::shared_ptr<networking::ClientConnection>) {

}

void Server::on_client_disconnected(std::shared_ptr<networking::ClientConnection>) {

}

void Server::handle_message(const networking::Message& message, std::shared_ptr<networking::ClientConnection> connection) {
    switch (message.id()) {
        case Client_Ping:
            m_server.get_logger()->debug("Client {} pinged us!", connection->get_id());
            break;
    }
}

void Server::server_ping() {
    m_server.send_message_all(networking::Message(Server_Ping));
}
