#pragma once

#include <glad/glad.h>
#include <entt/entt.hpp>

#include "nine_morris_3d_engine/graphics/renderer/buffer_layout.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/buffer.h"

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void bind();
    static void unbind();

    void add_buffer(entt::resource_handle<Buffer> buffer, const BufferLayout& layout);
    void add_index_buffer(entt::resource_handle<IndexBuffer> index_buffer);
private:
    GLuint array = 0;

    // These are just to hold a strong reference to the buffers
    // std::vector<entt::resource_handle<Buffer>> buffers;  // FIXME maybe not needed anymore
    // entt::resource_handle<IndexBuffer> index_buffer;
};
