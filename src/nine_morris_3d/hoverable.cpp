#include "nine_morris_3d/hoverable.h"

namespace hoverable {
    Id generate_id() {
        static Id id = 0;
        id++;
        return id;
    }
}
