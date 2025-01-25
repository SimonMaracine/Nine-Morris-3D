#pragma once

#include <utility>
#include <atomic>

#include <boost/endian/conversion.hpp>

#include "networking/internal/connection.hpp"

namespace networking {
    class Client;
}

namespace networking::internal {
    // Owner of this is the client
    class ServerConnection final : public Connection {
    public:
        ServerConnection(
            boost::asio::io_context& context,
            boost::asio::ip::tcp::socket&& tcp_socket,
            SyncQueue<Message>& incoming_messages,
            const boost::asio::ip::tcp::resolver::results_type& endpoints
        )
            : Connection(context, std::move(tcp_socket)), m_incoming_messages(incoming_messages),
            m_endpoints(endpoints) {}

        // Send a message asynchronously
        void send(const Message& message);
    private:
        void connect();
        bool connection_established() const noexcept;
        void add_to_incoming_messages();

        void task_write_header_payload();
        void task_read_header();
        void task_read_payload();
        void task_send_message(const Message& message);
        void task_connect_to_server();

        SyncQueue<Message>& m_incoming_messages;
        std::atomic_bool m_established_connection {false};
        boost::asio::ip::tcp::resolver::results_type m_endpoints;

        friend class ::networking::Client;
    };
}
