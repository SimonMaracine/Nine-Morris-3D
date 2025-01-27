#include "session_pool.hpp"

#include <cassert>

std::optional<protocol::SessionId> SessionPool::alloc_session_id() {
    const auto pointer {generate_pointer()};
    const bool right {generate_direction()};

    std::optional<protocol::SessionId> result;

    if (right) {
        result = search_and_alloc_id_right(pointer, SIZE);
    } else {
        result = search_and_alloc_id_left(0, pointer);
    }

    if (result) {
        return result;
    }

    // No ID found; start searching from the beginning

    if (right) {
        return search_and_alloc_id_right(0, pointer);
    } else {
        return search_and_alloc_id_left(pointer, SIZE);
    }

    // Return ID or null, if really nothing found
}

void SessionPool::free_session_id(protocol::SessionId session_id) {
    assert(m_pool[session_id]);

    m_pool[session_id] = false;
}

std::optional<protocol::SessionId> SessionPool::search_and_alloc_id_right(protocol::SessionId begin, protocol::SessionId end) {
    for (protocol::SessionId id {begin}; id < end; id++) {
        if (!m_pool[id]) {
            m_pool[id] = true;
            return id;
        }
    }

    return std::nullopt;
}

std::optional<protocol::SessionId> SessionPool::search_and_alloc_id_left(protocol::SessionId begin, protocol::SessionId end) {
    for (protocol::SessionId id {end}; id > begin; id--) {
        if (!m_pool[id]) {
            m_pool[id] = true;
            return id;
        }
    }

    return std::nullopt;
}

protocol::SessionId SessionPool::generate_pointer() {
    return m_distribution(m_random);
}

bool SessionPool::generate_direction() {
    return m_distribution(m_random) > SIZE / 2;
}
