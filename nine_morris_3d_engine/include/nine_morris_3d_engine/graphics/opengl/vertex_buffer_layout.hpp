#pragma once

#include <vector>
#include <cstddef>

namespace sm {
    struct VertexBufferLayout {
        enum Type : unsigned char {
            Float,
            Int
        };

        struct VertexElement {
            unsigned int index {};
            int size {};
            Type type {};
            bool per_instance {false};

            static std::size_t get_size(Type type) noexcept;
        };

        std::vector<VertexElement> elements;
        int stride {};

        VertexBufferLayout& add(unsigned int index, Type type, int size, bool per_instance = false);
    };
}
