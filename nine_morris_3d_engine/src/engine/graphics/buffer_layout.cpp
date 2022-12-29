#include <glad/glad.h>

#include "engine/graphics/buffer_layout.h"
#include "engine/other/logging.h"
#include "engine/other/exit.h"

size_t BufferLayout::VertexElement::get_size(Type type) {
    switch (type) {
        case Float:
            return sizeof(GLfloat);
        case Int:
            return sizeof(GLint);
        default:
            REL_CRITICAL("Type `{}` is not supported, exiting...", type);
            game_exit::exit_critical();
    }

    return 0;
}

void BufferLayout::add(GLuint index, Type type, GLint size, bool per_instance) {
    elements.push_back(VertexElement {index, type, size, per_instance});
    stride += size * VertexElement::get_size(type);
}
