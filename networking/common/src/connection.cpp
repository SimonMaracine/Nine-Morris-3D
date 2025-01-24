#include "networking/internal/connection.hpp"

namespace networking::internal {
    void Connection::close() {
        boost::asio::post(m_context, [this]() {
            if (!m_tcp_socket.is_open()) {
                return;
            }

            m_tcp_socket.close();
        });
    }

    bool Connection::is_open() const {
        return m_tcp_socket.is_open();
    }
}
