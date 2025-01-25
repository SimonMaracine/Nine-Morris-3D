#include "networking/internal/client_connection.hpp"

#include <vector>
#include <cstddef>
#include <cassert>

#include <boost/endian/conversion.hpp>

#include "networking/internal/error.hpp"

namespace networking::internal {
    void ClientConnection::send(const Message& message) {
        task_send_message(message);
    }

    ClientId ClientConnection::get_id() const noexcept {
        return m_client_id;
    }

    void ClientConnection::start_communication() {
        task_read_header();
    }

    void ClientConnection::add_to_incoming_messages() {
        m_incoming_messages.emplace_back(
            Message(m_incoming_message.header, std::move(m_incoming_message.payload)),
            shared_from_this()
        );

        m_incoming_message = {};
    }

    void ClientConnection::task_write_header_payload() {
        assert(!m_outgoing_messages.empty());

        MsgHeader header_to_write {m_outgoing_messages.front().header};
        boost::endian::native_to_big_inplace(header_to_write.id);
        boost::endian::native_to_big_inplace(header_to_write.payload_size);

        std::vector<boost::asio::const_buffer> buffers;
        buffers.emplace_back(&header_to_write, sizeof(MsgHeader));

        if (m_outgoing_messages.front().header.payload_size > 0) {
            buffers.emplace_back(m_outgoing_messages.front().payload.get(), m_outgoing_messages.front().header.payload_size);
        }

        const std::size_t size {buffers_size(buffers)};

        boost::asio::async_write(m_tcp_socket, buffers,
            [this, size](boost::system::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                if (ec) {
                    m_tcp_socket.close();

                    m_logger->error("[{}] Could not write header/payload: {}", get_id(), ec.message());
                    return;
                }

                assert(bytes_transferred == size);

                m_outgoing_messages.pop_front();

                // Thus writing tasks can stop
                if (!m_outgoing_messages.empty()) {
                    task_write_header_payload();
                }
            }
        );
    }

    void ClientConnection::task_read_header() {
        boost::asio::async_read(m_tcp_socket, boost::asio::buffer(&m_incoming_message.header, sizeof(MsgHeader)),
            [this](boost::system::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                if (ec) {
                    m_tcp_socket.close();

                    m_logger->error("[{}] Could not read header: {}", get_id(), ec.message());
                    return;
                }

                assert(bytes_transferred == sizeof(MsgHeader));

                boost::endian::big_to_native_inplace(m_incoming_message.header.id);
                boost::endian::big_to_native_inplace(m_incoming_message.header.payload_size);

                // Check if there is a payload to read
                if (m_incoming_message.header.payload_size > 0) {
                    // Allocate space so that we write to it later
                    m_incoming_message.payload = std::make_unique<unsigned char[]>(m_incoming_message.header.payload_size);

                    task_read_payload();
                } else {
                    add_to_incoming_messages();
                    task_read_header();
                }
            }
        );
    }

    void ClientConnection::task_read_payload() {
        boost::asio::async_read(m_tcp_socket, boost::asio::buffer(m_incoming_message.payload.get(), m_incoming_message.header.payload_size),
            [this](boost::system::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                if (ec) {
                    m_tcp_socket.close();

                    m_logger->error("[{}] Could not read payload: {}", get_id(), ec.message());
                    return;
                }

                assert(bytes_transferred == m_incoming_message.header.payload_size);

                add_to_incoming_messages();
                task_read_header();
            }
        );
    }

    void ClientConnection::task_send_message(const Message& message) {
        boost::asio::post(m_context,
            [this, message = message]() mutable {
                const bool writing_tasks_stopped {m_outgoing_messages.empty()};

                m_outgoing_messages.push_back(basic_message(std::move(message)));

                // Restart the writing process, if it has stopped before
                if (writing_tasks_stopped) {
                    task_write_header_payload();
                }
            }
        );
    }
}
