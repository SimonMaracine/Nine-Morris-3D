#include "engine/other/random_gen.h"
#include "engine/other/assert.h"

namespace random_gen {
    void initialize() {
        srand(time(nullptr));
    }

    int next() {
        return rand();
    }

    int next(int end) {
        ASSERT(end >= 0 && end <= RAND_MAX, "Invalid end value");

        return rand() % end;
    }

    int next(int begin, int end) {
        ASSERT(begin >= 0, "Invalid begin value");
        ASSERT(end >= 0 && end <= RAND_MAX, "Invalid end value");
        ASSERT(end > begin, "Invalid range");

        return rand() % (end - begin) + begin;
    }
}
