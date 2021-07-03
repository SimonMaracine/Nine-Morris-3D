#pragma once

#include <vector>
#include <cstddef>

#include <glad/glad.h>

struct VertexElement {
    GLuint index;
    GLenum type;
    GLint size;

    static std::size_t get_size(GLenum type);
};

struct BufferLayout {
    std::vector<VertexElement> elements;
    GLsizei stride = 0;

    enum class Type {
        Float, Int
    };

    void add(GLuint index, Type type, GLint size);
};
