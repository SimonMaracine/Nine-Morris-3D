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

        const int index = next(list.size());

        return list.begin()[index];
    }
}
