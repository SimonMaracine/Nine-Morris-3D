#pragma once

#include <memory>
#include <vector>

#include <glad/glad.h>

#include "graphics/renderer/buffer.h"
#include "graphics/renderer/buffer_layout.h"

class VertexArray {
public:
    VertexArray(GLuint array);
    ~VertexArray();

    static std::shared_ptr<VertexArray> create();

    void bind() const;
    static void unbind();

    void add_buffer(std::shared_ptr<Buffer> buffer, const BufferLayout& layout);
    void add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer);
private:
    GLuint array;

    // This is just to hold a strong reference to the buffers
    std::vector<std::shared_ptr<Buffer>> buffers;
    std::shared_ptr<IndexBuffer> index_buffer;
};

template<typename T>
using Rc = std::shared_ptr<T>;
