#pragma once

#include "engine/other/assert.h"

namespace sm {
    class RandomGenerator final {
    public:
        RandomGenerator();
        ~RandomGenerator() = default;

        RandomGenerator(const RandomGenerator&) = delete;
        RandomGenerator& operator=(const RandomGenerator&) = delete;
        RandomGenerator(RandomGenerator&&) = delete;
        RandomGenerator& operator=(RandomGenerator&&) = delete;

        // The range is inclusive: [a, b]

        uint32_t next();
        uint32_t next(uint32_t end);
        uint32_t next(uint32_t begin, uint32_t end);

        template<typename T>
        T choice(std::initializer_list<T> list) {
            ASSERT(list.size() > 0, "List must not be empty");

            const uint32_t index = next(list.size() - 1);

            return list.begin()[index];
        }

        template<typename T, typename Iter>
        T choice(Iter first, Iter last) {
            const size_t size = std::distance(first, last);

            ASSERT(size > 0, "List must not be empty");

            const uint32_t index = next(size - 1);

            return first[index];
        }
    private:
        std::mt19937 random;
        std::uniform_int_distribution<std::mt19937::result_type> default_distribution;
    };
}
