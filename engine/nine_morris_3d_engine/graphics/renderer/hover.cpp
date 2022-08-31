#include "nine_morris_3d_engine/graphics/renderer/hover.h"

namespace hover {
    Id generate_id() {
        static Id id = 0;
        id++;
        return id;
    }
}
