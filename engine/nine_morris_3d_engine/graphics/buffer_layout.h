#pragma once

#include <glad/glad.h>

struct BufferLayout {
    enum Type {
        Float, Int
    };

    struct VertexElement {
        GLuint index;
        Type type;
        GLint size;

        static size_t get_size(Type type);
    };

    std::vector<VertexElement> elements;
    GLsizei stride = 0;

    void add(GLuint index, Type type, GLint size);
};
