#include "nine_morris_3d_engine/graphics/identifier.h"

namespace identifier {
    static Id id = 0.0f;

    void initialize() {
        id = 0.0f;
    }

    Id generate_id() {
        id += 1.0f;
        return id;
    }

#if 0
    bool check(Id identifier_generated, Id identifier_unreliable) {
        constexpr float OFFSET = 0.1f;

        return (
            identifier_generated < identifier_unreliable + OFFSET
            && identifier_generated > identifier_unreliable - OFFSET
        );
    }
#endif
}
