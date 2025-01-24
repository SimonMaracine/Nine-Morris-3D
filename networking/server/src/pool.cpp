#include "networking/internal/pool.hpp"

#include <cassert>

namespace networking::internal {
    void Pool::create(ClientId pool_size) {
        m_pool = std::make_unique<bool[]>(pool_size);
        m_size = pool_size;
        m_pointer = 0;
    }

    std::optional<ClientId> Pool::alloc_id() {
        std::lock_guard lock {m_mutex};

        const auto result {search_and_alloc_id(m_pointer, m_size)};

        if (result) {
            return result;
        }

        // No ID found; start searching from the beginning

        return search_and_alloc_id(0, m_pointer);

        // Return ID or null, if really nothing found
    }

    void Pool::free_id(ClientId id) {
        std::lock_guard lock {m_mutex};

        assert(m_pool[id]);

        m_pool[id] = false;
    }

    std::optional<ClientId> Pool::search_and_alloc_id(ClientId begin, ClientId end) noexcept {
        for (ClientId id {begin}; id < end; id++) {
            if (!m_pool[id]) {
                m_pool[id] = true;
                m_pointer = (id + 1) % m_size;

                return id;
            }
        }

        return std::nullopt;
    }
}
