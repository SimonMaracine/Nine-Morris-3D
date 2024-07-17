#pragma once

#include <random>
#include <initializer_list>
#include <random>
#include <cstdint>
#include <cstddef>
#include <iterator>

namespace sm {
    class Ctx;

    // TODO remove this, use GLM's random functions instead and move these in utils
    class RandomGenerator {
    private:
        RandomGenerator();
    public:
        // The range is inclusive: [a, b]
        std::uint_fast32_t next();
        std::uint_fast32_t next(std::uint_fast32_t end);
        std::uint_fast32_t next(std::uint_fast32_t begin, std::uint_fast32_t end);

        template<typename T>
        T choice(std::initializer_list<T> list) {
            const std::uint_fast32_t index {choice(list.size())};

            return list.begin()[index];
        }

        template<typename T, typename Iter>
        T choice(Iter first, Iter last) {
            const std::uint_fast32_t index {choice(std::distance(first, last))};

            return first[index];
        }
    private:
        std::uint_fast32_t choice(std::size_t size);

        std::mt19937 random;

        friend class Ctx;
    };
}
