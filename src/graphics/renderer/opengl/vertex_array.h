#pragma once

#include <glad/glad.h>

#include "graphics/renderer/buffer_layout.h"
#include "graphics/renderer/opengl/buffer.h"

/**
 * This represents OpenGL vertex arrays, structures that keep track of buffers and their layout.
 */
class VertexArray {
public:
    VertexArray(GLuint array);
    ~VertexArray();

    static std::shared_ptr<VertexArray> create();

    void bind();
    static void unbind();

    void add_buffer(std::shared_ptr<Buffer> buffer, const BufferLayout& layout);
    void add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer);
private:
    GLuint array = 0;

    // These are just to hold a strong reference to the buffers
    std::vector<std::shared_ptr<Buffer>> buffers;
    std::shared_ptr<IndexBuffer> index_buffer;
};
