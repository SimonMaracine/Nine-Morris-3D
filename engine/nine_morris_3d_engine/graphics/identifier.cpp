#include "nine_morris_3d_engine/graphics/identifier.h"

namespace identifier {
    static Id id = 0;

    void initialize() {
        id = 0;
    }

    Id generate_id() {
        id++;
        return id;
    }
}
