#include "engine/graphics/identifier.hpp"

namespace sm {
    Id Identifier::generate() {
        id += 1.0f;

        return id;
    }
}
