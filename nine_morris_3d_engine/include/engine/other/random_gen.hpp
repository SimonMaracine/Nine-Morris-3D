#pragma once

#include <initializer_list>
#include <random>
#include <cstddef>
#include <cstdint>
#include <iterator>

#include "engine/other/assert.hpp"

namespace sm {
    class RandomGenerator {
    public:
        RandomGenerator();
        ~RandomGenerator() = default;

        RandomGenerator(const RandomGenerator&) = delete;
        RandomGenerator& operator=(const RandomGenerator&) = delete;
        RandomGenerator(RandomGenerator&&) = delete;
        RandomGenerator& operator=(RandomGenerator&&) = delete;

        // The range is inclusive: [a, b]

        std::uint32_t next();
        std::uint32_t next(std::uint32_t end);
        std::uint32_t next(std::uint32_t begin, std::uint32_t end);

        template<typename T>
        T choice(std::initializer_list<T> list) {
            SM_ASSERT(list.size() > 0, "List must not be empty");

            const std::uint32_t index {next(list.size() - 1)};

            return list.begin()[index];
        }

        template<typename T, typename Iter>
        T choice(Iter first, Iter last) {
            const std::size_t size {std::distance(first, last)};

            SM_ASSERT(size > 0, "List must not be empty");

            const std::uint32_t index {next(size - 1)};

            return first[index];
        }
    private:
        std::mt19937 random;
    };
}
