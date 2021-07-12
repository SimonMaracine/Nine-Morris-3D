#include <cstddef>

#include <glad/glad.h>

#include "opengl/renderer/buffer_layout.h"
#include "other/logging.h"

std::size_t VertexElement::get_size(GLenum type) {
    switch (type) {
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_INT:
            return sizeof(GLint);
        default:
            spdlog::critical("Type {} is not supported", type);
            std::exit(1);
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
    }

    elements.push_back({ index, gl_type, size });
    stride += size * VertexElement::get_size(gl_type);
}
