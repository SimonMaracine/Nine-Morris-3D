#pragma once

#include <optional>
#include <limits>
#include <random>

#include <protocol.hpp>

class SessionPool {
public:
    SessionPool()
        : m_random(std::random_device()()) {}

    std::optional<protocol::SessionId> alloc_session_id();
    void free_session_id(protocol::SessionId session_id);
private:
    std::optional<protocol::SessionId> search_and_alloc_id_right(protocol::SessionId begin, protocol::SessionId end);
    std::optional<protocol::SessionId> search_and_alloc_id_left(protocol::SessionId begin, protocol::SessionId end);
    protocol::SessionId generate_pointer();
    bool generate_direction();

    static constexpr protocol::SessionId SIZE {std::numeric_limits<protocol::SessionId>::max()};

    std::unique_ptr<bool[]> m_pool {std::make_unique<bool[]>(SIZE)};

    std::mt19937 m_random;
    std::uniform_int_distribution<protocol::SessionId> m_distribution;
};
