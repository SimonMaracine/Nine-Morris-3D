#include "engine/graphics/identifier.hpp"

namespace sm {
    Identifier::Id Identifier::generate() {
        id += 1.0f;

        return id;
    }

    Identifier::Id Identifier::null {0.0f};
}
