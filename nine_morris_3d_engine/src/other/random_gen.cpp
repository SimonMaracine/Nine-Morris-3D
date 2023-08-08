#include "engine/other/random_gen.h"
#include "engine/other/assert.h"

namespace sm {
    RandomGenerator::RandomGenerator() {
        std::random_device dev;
        random = std::mt19937(dev());
    }

    uint32_t RandomGenerator::next() {
        return default_distribution(random);
    }

    uint32_t RandomGenerator::next(uint32_t end) {
        std::uniform_int_distribution<std::mt19937::result_type> distribution {0, end};

        return distribution(random);
    }

    uint32_t RandomGenerator::next(uint32_t begin, uint32_t end) {
        ASSERT(end > begin, "Invalid range");

        std::uniform_int_distribution<std::mt19937::result_type> distribution {begin, end};

        return distribution(random);
    }
}
