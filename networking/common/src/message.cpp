#include "networking/internal/message.hpp"

#include <utility>
#include <cstring>

namespace networking::internal {
    BasicMessage basic_message(Message&& message) noexcept {
        BasicMessage result;
        result.header = message.m_header;
        result.payload = std::move(message.m_payload);

        return result;
    }

    Message::Message(std::uint16_t id) noexcept {
        m_header.id = id;
    }

    Message::Message(MsgHeader header, std::unique_ptr<unsigned char[]>&& payload) noexcept
        : m_header(header), m_payload(std::move(payload)) {}

    Message::Message(const Message& other) {
        if (other.m_payload != nullptr) {
            m_payload = std::make_unique<unsigned char[]>(other.m_header.payload_size);
            std::memcpy(m_payload.get(), other.m_payload.get(), other.m_header.payload_size);
        }

        m_header = other.m_header;
    }

    Message& Message::operator=(const Message& other) {
        if (other.m_payload != nullptr) {
            m_payload = std::make_unique<unsigned char[]>(other.m_header.payload_size);
            std::memcpy(m_payload.get(), other.m_payload.get(), other.m_header.payload_size);
        }

        m_header = other.m_header;

        return *this;
    }

    std::size_t Message::size() const noexcept {
        return sizeof(MsgHeader) + m_header.payload_size;
    }

    std::uint16_t Message::id() const noexcept {
        return m_header.id;
    }

    void Message::write_payload(std::string&& buffer) {
        m_payload = std::make_unique<unsigned char[]>(buffer.size());
        std::memcpy(m_payload.get(), reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
        m_header.payload_size = buffer.size();
    }
}
