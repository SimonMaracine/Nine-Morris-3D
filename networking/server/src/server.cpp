#include "networking/server.hpp"

#include <string>
#include <stdexcept>
#include <cassert>

namespace networking {
    Server::Server(
        std::function<void(std::shared_ptr<ClientConnection>)> on_client_connected,
        std::function<void(std::shared_ptr<ClientConnection>)> on_client_disconnected,
        unsigned int log_target,
        const std::filesystem::path& log_file_path
    )
        : m_acceptor(m_context), m_on_client_connected(std::move(on_client_connected)),
        m_on_client_disconnected(std::move(on_client_disconnected)) {
        initialize_logging(log_target, log_file_path);
    }

    Server::~Server() {
        stop();
    }

    void Server::start(std::uint16_t port, std::uint32_t max_clients) {
        if (m_context.stopped()) {
            m_context.restart();
        }

        m_pool.create(max_clients);

        const auto endpoint {boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)};

        try {
            m_acceptor.open(endpoint.protocol());
            m_acceptor.bind(endpoint);
            m_acceptor.listen();
        } catch (const boost::system::system_error& e) {
            m_logger->critical("Unexpected error: {}", e.what());
            throw ConnectionError(e.what());
        }

        m_running = true;

        task_accept_connection();

        m_context_thread = std::thread([this]() {
            try {
                m_context.run();
            } catch (const boost::system::system_error& e) {
                m_logger->critical("Unexpected error: {}", e.what());
                m_error = std::make_exception_ptr(ConnectionError(e.what()));
            } catch (const ConnectionError& e) {
                m_logger->critical("Unexpected error: {}", + e.what());
                m_error = std::current_exception();
            }
        });

        m_logger->info("Server started (port {}, max {} clients)", port, max_clients);
    }

    void Server::stop() {
        m_running = false;

        // Don't prime the context, if it has been stopped,
        // because it will do the work after restart, meaning use after free
        if (!m_context.stopped()) {
            for (const auto& connection : m_connections) {
                if (connection != nullptr) {
                    connection->close();
                }
            }
        }

        if (m_acceptor.is_open()) {
            try {
                m_acceptor.close();
            } catch (const boost::system::system_error&) {}
        }

        if (m_context_thread.joinable()) {
            m_context_thread.join();
        }

        for (auto& connection : m_connections) {
            connection.reset();
        }

        m_connections.clear();
        m_new_connections.clear();
        m_incoming_messages.clear();

        m_error = nullptr;
    }

    void Server::accept_connections() {
        throw_if_error();

        // This function may only pop m_new_connections and the accepting thread may only push m_new_connections

        while (!m_new_connections.empty()) {
            const auto connection {m_new_connections.pop_front()};

            m_on_client_connected(connection);
            m_connections.push_front(connection);
            connection->start_communication();
        }
    }

    void Server::check_connections() {
        throw_if_error();

        for (auto before_iter {m_connections.before_begin()}, iter {m_connections.begin()}; iter != m_connections.end();) {
            const auto& connection {*iter};

            assert(connection != nullptr);

            if (!connection->is_open()) {
                maybe_client_disconnected(connection, iter, before_iter);
                continue;
            }

            before_iter++, iter++;
        }
    }

    std::pair<std::shared_ptr<ClientConnection>, Message> Server::next_message() {
        return m_incoming_messages.pop_front();
    }

    bool Server::available_messages() const {
        return !m_incoming_messages.empty();
    }

    void Server::send_message(std::shared_ptr<ClientConnection> connection, const Message& message) {
        throw_if_error();

        assert(connection != nullptr);

        if (!connection->is_open()) {
            maybe_client_disconnected(connection);
            return;
        }

        connection->send(message);
    }

    void Server::send_message_all(const Message& message) {
        throw_if_error();

        for (auto before_iter {m_connections.before_begin()}, iter {m_connections.begin()}; iter != m_connections.end();) {
            const auto& connection {*iter};

            assert(connection != nullptr);

            if (!connection->is_open()) {
                maybe_client_disconnected(connection, iter, before_iter);
                continue;
            }

            connection->send(message);

            before_iter++, iter++;
        }
    }

    void Server::send_message_all(const Message& message, std::shared_ptr<ClientConnection> exception) {
        throw_if_error();

        for (auto before_iter {m_connections.before_begin()}, iter {m_connections.begin()}; iter != m_connections.end();) {
            const auto& connection {*iter};

            assert(connection != nullptr);

            if (connection == exception) {
                continue;
            }

            if (!connection->is_open()) {
                maybe_client_disconnected(connection, iter, before_iter);
                continue;
            }

            connection->send(message);

            before_iter++, iter++;
        }
    }

    void Server::throw_if_error() {
        if (!m_error) {
            return;
        }

        const auto error {m_error};
        stop();
        std::rethrow_exception(error);
    }

    void Server::task_accept_connection() {
        // IDs are allocated in this thread, but they are freed in the main thread

        m_acceptor.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
                if (ec) {
                    m_logger->warn("Could not accept new connection: {}", ec.message());
                } else {
                    m_logger->info(
                        "Accepted new connection: {}, {}",
                        socket.remote_endpoint().address().to_string(),
                        std::to_string(socket.remote_endpoint().port())
                    );

                    const auto new_id {m_pool.alloc_id()};

                    if (!new_id) {
                        socket.close();

                        m_logger->error("Actively rejected connection: ran out of IDs");
                    } else {
                        m_new_connections.push_back(
                            std::make_shared<ClientConnection>(
                                m_context,
                                std::move(socket),
                                m_incoming_messages,
                                *new_id,
                                m_logger
                            )
                        );
                    }
                }

                if (!m_running) {
                    return;
                }

                task_accept_connection();
            }
        );
    }

    void Server::maybe_client_disconnected(std::shared_ptr<ClientConnection> connection) {
        if (connection->m_used) {
            return;
        }

        connection->m_used = true;  // Avoid calling on_client_disconnected again
        m_connections.remove(connection);
        m_on_client_disconnected(connection);
        m_pool.free_id(connection->get_id());
    }

    void Server::maybe_client_disconnected(std::shared_ptr<ClientConnection> connection, ConnectionsIter& iter, ConnectionsIter before_iter) {
        if (connection->m_used) {
            return;
        }

        connection->m_used = true;  // Avoid calling on_client_disconnected again
        iter = m_connections.erase_after(before_iter);
        m_on_client_disconnected(connection);
        m_pool.free_id(connection->get_id());
    }

    void Server::initialize_logging(unsigned int log_target, const std::filesystem::path& log_file_path) {
        m_logger = std::make_shared<spdlog::logger>("ServerLogger");

        try {
            if (log_target & LogTargetConsole) {
                m_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                m_console_sink->set_pattern("%^[%l] [%H:%M:%S]%$ %v");
                m_console_sink->set_level(spdlog::level::trace);

                m_logger->sinks().push_back(m_console_sink);
            }

            if (log_target & LogTargetFile) {
                static constexpr int MAX_SIZE {1024 * 1024 * 5};
                static constexpr int MAX_FILES {5};

                m_rotating_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file_path.string(), MAX_SIZE, MAX_FILES);
                m_rotating_file_sink->set_pattern("%^[%l] [%H:%M:%S]%$ %v");
                m_rotating_file_sink->set_level(spdlog::level::trace);

                m_logger->sinks().push_back(m_rotating_file_sink);
            }
        } catch (const spdlog::spdlog_ex& e) {
            throw ServerError("Could not initialize logging: " + std::string(e.what()));
        }
    }
}
