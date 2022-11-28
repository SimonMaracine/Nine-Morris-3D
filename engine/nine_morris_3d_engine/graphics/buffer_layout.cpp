#include <glad/glad.h>

#include "nine_morris_3d_engine/graphics/buffer_layout.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/exit.h"

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

void BufferLayout::add(GLuint index, Type type, GLint size) {
    elements.push_back(VertexElement {index, type, size});
    stride += size * VertexElement::get_size(type);
}
