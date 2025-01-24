#pragma once

#include <utility>
#include <memory>
#include <functional>

#include "networking/internal/connection.hpp"
#include "networking/internal/id.hpp"

namespace networking {
    class Server;
}

namespace networking::internal {
    // Owner of this is the server
    class ClientConnection final : public Connection, public std::enable_shared_from_this<ClientConnection> {
    public:
        ClientConnection(
            boost::asio::io_context& context,
            boost::asio::ip::tcp::socket&& tcp_socket,
            SyncQueue<std::pair<Message, std::shared_ptr<ClientConnection>>>& incoming_messages,
            ClientId client_id
        )
            : Connection(context, std::move(tcp_socket)), m_incoming_messages(incoming_messages), m_client_id(client_id) {}

        // Send a message asynchronously
        void send(const Message& message);

        // Get the unique ID of this client
        ClientId get_id() const noexcept;
    private:
        void start_communication();
        void add_to_incoming_messages();

        void task_write_header_payload();
        void task_read_header();
        void task_read_payload();
        void task_send_message(const Message& message);

        SyncQueue<std::pair<Message, std::shared_ptr<ClientConnection>>>& m_incoming_messages;
        ClientId m_client_id {};  // Given by the server
        bool m_used {false};  // Set to true after using the connection and calling on_client_disconnected()

        friend class ::networking::Server;
    };
}
