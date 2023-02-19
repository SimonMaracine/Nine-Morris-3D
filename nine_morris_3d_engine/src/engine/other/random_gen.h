#pragma once

#include "engine/other/assert.h"

namespace random_gen {
    void initialize();
    int next();
    int next(int end);
    int next(int begin, int end);

    template<typename T>
    T choice(std::initializer_list<T> list) {
        ASSERT(list.size() > 0, "List must not be empty");
        ASSERT(list.size() <= INT_MAX, "Size is too large");

        const int index = next(list.size());

        return list.begin()[index];
    }

    template<typename T, typename Iter>
    T choice(Iter first, Iter last) {
        const size_t size = std::distance(first, last);

        ASSERT(size > 0, "List must not be empty");
        ASSERT(size <= INT_MAX, "Size is too large");

        const int index = next(size);

        return first[index];
    }
}
