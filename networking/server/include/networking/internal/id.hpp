#pragma once

#include <cstdint>

namespace networking::internal {
    // Unique integer used to identify clients
    // Identifiers are managed automatically by the server
    using ClientId = std::uint32_t;
}
