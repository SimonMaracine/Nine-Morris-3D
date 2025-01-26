#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <limits>
#include <sstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "networking/internal/error.hpp"

namespace networking::internal {
    class Message;

    inline constexpr std::size_t MAX_ITEM_SIZE {std::numeric_limits<std::uint16_t>::max()};

    struct MsgHeader final {
        std::uint16_t id {};
        std::uint16_t payload_size {};
    };

    static_assert(std::is_trivially_copyable_v<MsgHeader>);

    struct BasicMessage final {
        MsgHeader header;
        std::unique_ptr<unsigned char[]> payload;
    };

    BasicMessage basic_message(Message&& message) noexcept;

    // Class representing a message, a blob of data
    // Message payload can be any data that cereal supports
    // If the payload is not written into from one side, it should not be read from the other side
    class Message final {
    public:
        Message() noexcept = default;
        explicit Message(std::uint16_t id) noexcept;
        Message(MsgHeader header, std::unique_ptr<unsigned char[]>&& payload) noexcept;

        ~Message() noexcept = default;

        Message(const Message& other);
        Message& operator=(const Message& other);
        Message(Message&&) noexcept = default;
        Message& operator=(Message&&) noexcept = default;

        // Get the size of the message (including header and payload)
        std::size_t size() const noexcept;

        // Get the ID of the message
        std::uint16_t id() const noexcept;

        // Write a serializable struct into the payload
        template<typename Payload>
        void write(const Payload& payload) {
            std::ostringstream stream {std::ios_base::binary};

            try {
                cereal::PortableBinaryOutputArchive archive {stream};
                archive(payload);
            } catch (const cereal::Exception& e) {
                throw SerializationError(e.what());
            }

            write_payload(stream.str());
        }

        // Read a serializable struct from the payload
        template<typename Payload>
        void read(Payload& payload) const {
            std::istringstream stream {std::ios_base::binary};
            stream.str(std::string(reinterpret_cast<char*>(m_payload.get()), m_header.payload_size));

            if (!stream.good()) {
                throw SerializationError("Stream is not in a good state");
            }

            try {
                cereal::PortableBinaryInputArchive archive {stream.seekg(0, std::ios_base::beg)};
                archive(payload);
            } catch (const cereal::Exception& e) {
                throw SerializationError(e.what());
            }
        }
    private:
        void write_payload(std::string&& buffer);

        MsgHeader m_header;
        std::unique_ptr<unsigned char[]> m_payload;

        friend BasicMessage basic_message(Message&& message) noexcept;
    };
}
