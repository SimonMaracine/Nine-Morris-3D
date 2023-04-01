#include <glad/glad.h>

#include "engine/graphics/vertex_buffer_layout.h"
#include "engine/other/logging.h"
#include "engine/application/panic.h"

size_t VertexBufferLayout::VertexElement::get_size(Type type) {
    switch (type) {
        case Float:
            return sizeof(GLfloat);
        case Int:
            return sizeof(GLint);
        default:
            LOG_DIST_CRITICAL("Type `{}` is not supported, exiting...", type);
            panic::panic();
    }

    return 0;
}

VertexBufferLayout& VertexBufferLayout::add(GLuint index, Type type, GLint size, bool per_instance) {
    elements.push_back(VertexElement {index, type, size, per_instance});
    stride += size * VertexElement::get_size(type);

    return *this;
}
