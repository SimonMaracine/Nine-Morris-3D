#pragma once

#include <cstddef>
#include <vector>

namespace sm {
    struct VertexBufferLayout {
        enum Type {
            Float,
            Int
        };

        struct VertexElement {
            unsigned int index = 0;
            Type type {};
            int size = 0;
            bool per_instance = false;

            static size_t get_size(Type type);
        };

        std::vector<VertexElement> elements;
        int stride = 0;

        VertexBufferLayout& add(unsigned int index, Type type, int size, bool per_instance = false);
    };
}
