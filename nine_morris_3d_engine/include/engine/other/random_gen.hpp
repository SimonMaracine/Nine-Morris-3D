#pragma once

#include <initializer_list>
#include <random>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <cassert>

namespace sm {
    class RandomGenerator {
    public:
        RandomGenerator();

        // The range is inclusive: [a, b]
        std::uint32_t next();
        std::uint32_t next(std::uint32_t end);
        std::uint32_t next(std::uint32_t begin, std::uint32_t end);

        template<typename T>
        T choice(std::initializer_list<T> list) {
            assert(list.size() > 0);

            const std::uint32_t index {next(list.size() - 1)};

            return list.begin()[index];
        }

        template<typename T, typename Iter>
        T choice(Iter first, Iter last) {
            const std::size_t size {std::distance(first, last)};

            assert(size > 0);

            const std::uint32_t index {next(size - 1)};

            return first[index];
        }
    private:
        std::mt19937 random;
    };
}
