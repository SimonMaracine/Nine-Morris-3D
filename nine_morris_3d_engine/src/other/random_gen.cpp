#include "engine/other/random_gen.hpp"

namespace sm {
    RandomGenerator::RandomGenerator() {
        std::random_device dev;
        random = std::mt19937(dev());
    }

    std::uint_fast32_t RandomGenerator::next() {
        std::uniform_int_distribution<std::mt19937::result_type> distribution {};

        return distribution(random);
    }

    std::uint_fast32_t RandomGenerator::next(std::uint_fast32_t end) {
        std::uniform_int_distribution<std::mt19937::result_type> distribution {0, end};

        return distribution(random);
    }

    std::uint_fast32_t RandomGenerator::next(std::uint_fast32_t begin, std::uint_fast32_t end) {
        assert(end > begin);

        std::uniform_int_distribution<std::mt19937::result_type> distribution {begin, end};

        return distribution(random);
    }
}
