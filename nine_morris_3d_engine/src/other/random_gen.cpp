#include <initializer_list>
#include <random>
#include <cstddef>
#include <cstdint>

#include "engine/other/random_gen.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    RandomGenerator::RandomGenerator() {
        std::random_device dev;
        random = std::mt19937(dev());
    }

    std::uint32_t RandomGenerator::next() {
        return default_distribution(random);
    }

    std::uint32_t RandomGenerator::next(std::uint32_t end) {
        std::uniform_int_distribution<std::mt19937::result_type> distribution {0, end};

        return distribution(random);
    }

    std::uint32_t RandomGenerator::next(std::uint32_t begin, std::uint32_t end) {
        SM_ASSERT(end > begin, "Invalid range");

        std::uniform_int_distribution<std::mt19937::result_type> distribution {begin, end};

        return distribution(random);
    }
}
