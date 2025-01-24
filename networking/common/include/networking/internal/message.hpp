#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <limits>

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

    BasicMessage basic_message(Message&& message);

    // Class representing a message, a blob of data
    // Messages can only contain data from trivially copyable types
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

        // Get the size of the message, including header and payload
        std::size_t size() const noexcept;

        // Get the message ID
        std::uint16_t id() const noexcept;
    private:
        MsgHeader m_header;
        std::unique_ptr<unsigned char[]> m_payload;

        friend BasicMessage basic_message(Message&& message);
    };
}
