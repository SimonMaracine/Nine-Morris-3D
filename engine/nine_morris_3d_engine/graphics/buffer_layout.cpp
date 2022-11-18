#include <glad/glad.h>

#include "nine_morris_3d_engine/graphics/buffer_layout.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/exit.h"

size_t VertexElement::get_size(GLenum type) {
    switch (type) {
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_INT:
            return sizeof(GLint);
        default:
            REL_CRITICAL("Type {} is not supported, exiting...", type);
            game_exit::exit_critical();
            return 0;
    }
}

void BufferLayout::add(GLuint index, Type type, GLint size) {
    GLenum gl_type;

    switch (type) {
        case Float:
            gl_type = GL_FLOAT;
            break;
        case Int:
            gl_type = GL_INT;
            break;
        default:
            REL_CRITICAL("Unrecognized type, exiting...");
            game_exit::exit_critical();
    }

    elements.push_back({ index, gl_type, size });
    stride += size * VertexElement::get_size(gl_type);
}
