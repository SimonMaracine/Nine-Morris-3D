#pragma once

#include <utility>
#include <cstddef>

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <boost/asio.hpp>

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

#include "networking/internal/message.hpp"
#include "networking/internal/queue.hpp"

namespace networking::internal {
    class Connection {
    protected:
        Connection(boost::asio::io_context& context, boost::asio::ip::tcp::socket&& tcp_socket)
            : m_context(context), m_tcp_socket(std::move(tcp_socket)) {}

        ~Connection() = default;

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;
        Connection(Connection&&) = delete;
        Connection& operator=(Connection&&) = delete;

        void close();
        bool is_open() const;

        boost::asio::io_context& m_context;
        boost::asio::ip::tcp::socket m_tcp_socket;

        SyncQueue<BasicMessage> m_outgoing_messages;
        BasicMessage m_incoming_message;
    };

    template<typename T>
    std::size_t buffers_size(const T& buffers) noexcept {
        std::size_t size {0};

        for (const auto& buffer : buffers) {
            size += buffer.size();
        }

        return size;
    }
}
