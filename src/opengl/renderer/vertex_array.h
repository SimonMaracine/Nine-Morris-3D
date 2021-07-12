#pragma once

#include <memory>

#include <glad/glad.h>

#include "opengl/renderer/vertex_buffer.h"
#include "opengl/renderer/buffer_layout.h"

class VertexArray {
public:
    VertexArray(GLuint array);
    ~VertexArray();

    static std::shared_ptr<VertexArray> create();

    void bind() const;
    static void unbind();

    void add_buffer(std::shared_ptr<VertexBuffer> buffer,
                    const BufferLayout& layout);
private:
    GLuint array;
};
