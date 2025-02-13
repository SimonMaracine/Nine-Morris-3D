#include "client.hpp"

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

static constexpr const char* DEFAULT_ADDRESS {"localhost"};
static constexpr std::uint16_t DEFAULT_PORT {7915};

void Client::connect(const std::string& address, std::uint16_t port) {
    m_attempted_connection = true;

    m_client.disconnect();

    LOG_DIST_INFO("Connecting to {}:{}...", address, port);

    try {
        m_client.connect(address, port);
    } catch (const networking::ConnectionError&) {
        m_connection_state = ConnectionState::Disconnected;
        throw;
    }

    m_connection_state = ConnectionState::Connecting;
}

void Client::connect() {
    connect(DEFAULT_ADDRESS, DEFAULT_PORT);
}

void Client::disconnect() {
    m_connection_state = ConnectionState::Disconnected;
    m_client.disconnect();

    LOG_DIST_INFO("Disconnected from server");
}

bool Client::connection_established() {
    try {
        if (m_client.connection_established()) {
            LOG_DIST_INFO("Connected to the server");
            m_connection_state = ConnectionState::Connected;
            return true;
        }
    } catch (const networking::ConnectionError&) {
        m_connection_state = ConnectionState::Disconnected;
        throw;
    }

    return false;
}

bool Client::available_messages() const {
    return m_client.available_messages();
}

networking::Message Client::next_message() {
    return m_client.next_message();
}

void Client::send_message(const networking::Message& message) {
    try {
        m_client.send_message(message);
    } catch (const networking::ConnectionError&) {
        m_connection_state = ConnectionState::Disconnected;
        throw;
    }
}
