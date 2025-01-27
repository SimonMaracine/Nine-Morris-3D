#pragma once

#include <optional>
#include <memory>
#include <mutex>

#include "networking/internal/id.hpp"

namespace networking::internal {
    class Pool final {
    public:
        void create(ClientId size);
        std::optional<ClientId> alloc_id();
        void free_id(ClientId id);
    private:
        std::optional<ClientId> search_and_alloc_id(ClientId begin, ClientId end) noexcept;

        std::unique_ptr<bool[]> m_pool;  // False means it's not allocated
        ClientId m_size {};
        ClientId m_pointer {};
        std::mutex m_mutex;
    };
}
