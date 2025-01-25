#pragma once

#include <networking/server.hpp>

class Server {
public:
    Server();

    void start();
    void update();
private:
    void on_client_connected(std::shared_ptr<networking::ClientConnection> connection);
    void on_client_disconnected(std::shared_ptr<networking::ClientConnection> connection);

    void handle_message(networking::Message&& message, std::shared_ptr<networking::ClientConnection>&& connection);

    networking::Server m_server;
};
