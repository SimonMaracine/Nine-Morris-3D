#include "server.hpp"

Server::Server()
    : m_server(
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_connected(connection); },
        [this](std::shared_ptr<networking::ClientConnection> connection) { on_client_disconnected(connection); }
    ) {}

void Server::start() {
    m_server.start(7915);
}

void Server::update() {
    m_server.accept_connections();

    while (m_server.available_messages()) {
        auto [message, connection] {m_server.next_message()};
        handle_message(std::move(message), std::move(connection));
    }
}

void Server::on_client_connected(std::shared_ptr<networking::ClientConnection> connection) {

}

void Server::on_client_disconnected(std::shared_ptr<networking::ClientConnection> connection) {

}

void Server::handle_message(networking::Message&& message, std::shared_ptr<networking::ClientConnection>&& connection) {

}
