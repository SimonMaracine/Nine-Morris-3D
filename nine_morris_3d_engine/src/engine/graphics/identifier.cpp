#include "engine/graphics/identifier.h"

Identifier::Id Identifier::generate() {
    id += 1.0f;

    return id;
}

#if 0
bool Identifier::check(Id identifier_generated, Id identifier_unreliable) {
    static constexpr float OFFSET = 0.1f;

    return (
        identifier_generated < identifier_unreliable + OFFSET
        && identifier_generated > identifier_unreliable - OFFSET
    );
}
#endif

Identifier::Id Identifier::null = 0.0f;
