#pragma once

#include <resmanager/resmanager.hpp>

// Generic ID

namespace sm {
    using Id = resmanager::HashedStr64;
    using Hash = resmanager::Hash<Id>;
}
