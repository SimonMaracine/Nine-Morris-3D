#pragma once

#include <string>
#include <cstdint>

#include <networking/client.hpp>

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected
};

class Client {
public:
    ConnectionState get_connection_state() const { return m_connection_state; }
    bool get_attempted_connection() const { return m_attempted_connection; }

    void connect(const std::string& address, std::uint16_t port);
    void connect();
    void disconnect();
    bool connection_established();
    bool available_messages() const;
    networking::Message next_message();
    void send_message(const networking::Message& message);
private:
    networking::Client m_client;
    ConnectionState m_connection_state {ConnectionState::Disconnected};
    bool m_attempted_connection {false};  // Set to true after the very first connection attempt
};
