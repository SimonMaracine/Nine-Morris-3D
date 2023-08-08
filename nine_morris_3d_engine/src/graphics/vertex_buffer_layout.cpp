#include <cstddef>
#include <vector>

#include <glad/glad.h>

#include "engine/application_base/panic.hpp"
#include "engine/graphics/vertex_buffer_layout.hpp"
#include "engine/other/logging.hpp"

namespace sm {
    size_t VertexBufferLayout::VertexElement::get_size(Type type) {
        switch (type) {
            case Float:
                return sizeof(GLfloat);
            case Int:
                return sizeof(GLint);
        }

        return 0;
    }

    VertexBufferLayout& VertexBufferLayout::add(unsigned int index, Type type, int size, bool per_instance) {
        elements.push_back(VertexElement(index, type, size, per_instance));
        stride += size * VertexElement::get_size(type);

        return *this;
    }
}
