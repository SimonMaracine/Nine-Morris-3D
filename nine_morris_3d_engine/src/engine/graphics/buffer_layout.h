#pragma once

#include <glad/glad.h>

struct BufferLayout {
    enum Type {
        Invalid, Float, Int
    };

    struct VertexElement {
        GLuint index = 0;
        Type type = Invalid;
        GLint size = 0;
        bool per_instance = false;

        static size_t get_size(Type type);
    };

    std::vector<VertexElement> elements;
    GLsizei stride = 0;

    void add(GLuint index, Type type, GLint size, bool per_instance = false);
};
