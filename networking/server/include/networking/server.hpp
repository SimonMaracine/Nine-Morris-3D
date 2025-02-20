#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <forward_list>
#include <limits>
#include <utility>
#include <functional>
#include <exception>
#include <stdexcept>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "networking/internal/client_connection.hpp"
#include "networking/internal/queue.hpp"
#include "networking/internal/pool.hpp"
#include "networking/internal/message.hpp"

// Forward
#include "networking/internal/error.hpp"

namespace networking {
    using ClientConnection = internal::ClientConnection;
    using Message = internal::Message;
    using ConnectionError = internal::ConnectionError;
    using SerializationError = internal::SerializationError;
    using ClientId = internal::ClientId;

    // Used to specify where logs are emitted
    enum LogTarget : unsigned int {
        LogTargetNone = 0,
        LogTargetConsole = 1u << 0,
        LogTargetFile = 1u << 1
    };

    // Main class for the server program
    class Server final {
    public:
        // Sending messages or calling check_connections is prohibited in on_client_disconnected
        // Throws server errors
        Server(
            std::function<void(std::shared_ptr<ClientConnection>)> on_client_connected,
            std::function<void(std::shared_ptr<ClientConnection>)> on_client_disconnected,
            unsigned int log_target,
            const std::filesystem::path& log_file_path = "logs/rotating.log"
        );

        ~Server();

        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;

        // Start the internal event loop and start accepting connection requests
        // You may call this only once in the beginning or after calling stop()
        // Specify the port number on which to listen and the maximum amount of clients allowed
        // Throws connection
        void start(std::uint16_t port, std::uint32_t max_clients = std::numeric_limits<std::uint16_t>::max());

        // Disconnect from all the clients and stop the internal event loop
        // You may call this at any time
        // After a call to stop(), you may restart by calling start() again
        // It is automatically called in the destructor and when a fatal error occurrs
        void stop();

        // Accept new connections; you must call this regularly
        // Invokes on_client_connected() when needed
        // Throws connection errors
        void accept_connections();

        // Check the state of all connections
        // Invokes on_client_disconnected() when needed
        // Throws connection errors
        void check_connections();

        // Poll the next incoming message from the queue
        // You may call it in a loop to process as many messages as you want
        std::pair<std::shared_ptr<ClientConnection>, Message> next_message();

        // Check if there are available incoming messages
        bool available_messages() const;

        // Send a message to a specific client; invokes on_client_disconnected() when needed
        // Throws connection errors
        void send_message(std::shared_ptr<ClientConnection> connection, const Message& message);

        // Send a message to all clients; invokes on_client_disconnected() when needed
        // Throws connection errors
        void send_message_all(const Message& message);

        // Send a message to all clients except a specific client; invokes on_client_disconnected() when needed
        // Throws connection errors
        void send_message_all(const Message& message, std::shared_ptr<ClientConnection> exception);

        // Get a pointer to the logger
        std::shared_ptr<spdlog::logger> get_logger() { return m_logger; }
    private:
        using ConnectionsIter = std::forward_list<std::shared_ptr<ClientConnection>>::iterator;

        void throw_if_error();
        void task_accept_connection();
        void maybe_client_disconnected(std::shared_ptr<ClientConnection> connection);
        void maybe_client_disconnected(std::shared_ptr<ClientConnection> connection, ConnectionsIter& iter, ConnectionsIter before_iter);
        void initialize_logging(unsigned int log_target, const std::filesystem::path& log_file_path);

        std::forward_list<std::shared_ptr<ClientConnection>> m_connections;
        internal::SyncQueue<std::shared_ptr<ClientConnection>> m_new_connections;
        internal::SyncQueue<std::pair<std::shared_ptr<ClientConnection>, Message>> m_incoming_messages;

        std::thread m_context_thread;
        boost::asio::io_context m_context;
        boost::asio::ip::tcp::acceptor m_acceptor;

        std::function<void(std::shared_ptr<ClientConnection>)> m_on_client_connected;
        std::function<void(std::shared_ptr<ClientConnection>)> m_on_client_disconnected;

        internal::Pool m_pool;
        std::exception_ptr m_error;
        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> m_console_sink;
        std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> m_rotating_file_sink;
        bool m_running {false};
    };

    // Generic error thrown by the server
    struct ServerError : std::runtime_error {
        explicit ServerError(const char* message)
            : std::runtime_error(message) {}
        explicit ServerError(const std::string& message)
            : std::runtime_error(message) {}
    };
}
