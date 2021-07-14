#pragma once

#include <memory>
#include <vector>

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
    void hold_index_buffer(std::shared_ptr<VertexBuffer> index_buffer);
private:
    GLuint array;

    // This just to hold a strong reference to the buffers
    std::vector<std::shared_ptr<VertexBuffer>> buffers;
};
