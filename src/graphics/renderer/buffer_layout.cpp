#include <cstddef>

#include <glad/glad.h>

#include "graphics/renderer/buffer_layout.h"
#include "other/logging.h"

std::size_t VertexElement::get_size(GLenum type) {
    switch (type) {
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_INT:
            return sizeof(GLint);
        default:
            REL_CRITICAL("Type {} is not supported, exiting...", type);
            exit(1);
    }
}

void BufferLayout::add(GLuint index, Type type, GLint size) {
    GLenum gl_type;

    switch (type) {
        case Type::Float:
            gl_type = GL_FLOAT;
            break;
        case Type::Int:
            gl_type = GL_INT;
            break;
        default:
            REL_CRITICAL("Unrecognized type, exiting...");
            exit(1);
    }

    elements.push_back({ index, gl_type, size });
    stride += size * VertexElement::get_size(gl_type);
}
