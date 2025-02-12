#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"

#include <cassert>

#include <glad/glad.h>

#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    GlVertexArray::GlVertexArray() {
        glGenVertexArrays(1, &m_array);
        glBindVertexArray(m_array);

        glBindVertexArray(0);

        LOG_DEBUG("Created GL vertex array {}", m_array);
    }

    GlVertexArray::~GlVertexArray() {
        glDeleteVertexArrays(1, &m_array);

        LOG_DEBUG("Deleted GL vertex array {}", m_array);
    }

    void GlVertexArray::bind() const {
        glBindVertexArray(m_array);
    }

    void GlVertexArray::unbind() {
        glBindVertexArray(0);
    }

    void GlVertexArray::configure(const Configuration& configuration) {
        bind();
        configuration(this);
        unbind();

        if (!m_index_buffers.empty()) {
            assert(m_index_buffer_index != INVALID_INDEX_BUFFER);
        }

        // Unbind index buffer after vertex array
        GlIndexBuffer::unbind();
    }

    void GlVertexArray::add_vertex_buffer(std::shared_ptr<GlVertexBuffer> vertex_buffer, const VertexBufferLayout& layout) {
        assert(layout.elements.size() > 0);

        vertex_buffer->bind();

        std::size_t offset {0};

        for (std::size_t i {0}; i < layout.elements.size(); i++) {
            const VertexBufferLayout::VertexElement& element {layout.elements[i]};

            switch (element.type) {
                case VertexBufferLayout::Float:
                    glVertexAttribPointer(
                        element.index,
                        element.size,
                        GL_FLOAT,
                        GL_FALSE,
                        layout.stride,
                        reinterpret_cast<void*>(offset)
                    );
                    break;
                case VertexBufferLayout::Int:
                    glVertexAttribIPointer(
                        element.index,
                        element.size,
                        GL_INT,
                        layout.stride,
                        reinterpret_cast<void*>(offset)
                    );
                    break;
            }

            glEnableVertexAttribArray(element.index);

            if (element.per_instance) {
                glVertexAttribDivisor(element.index, 1);
            }

            offset += element.size * VertexBufferLayout::VertexElement::get_size(element.type);
        }

        m_vertex_buffers.push_back(vertex_buffer);

        GlVertexBuffer::unbind();
    }

    void GlVertexArray::add_index_buffer(std::shared_ptr<GlIndexBuffer> index_buffer) {
        m_index_buffers.push_back(index_buffer);
    }

    void GlVertexArray::bind_index_buffer(std::size_t index) {
        assert(m_index_buffers.size() > index);

        m_index_buffers[index]->bind();
        m_index_buffer_index = index;
    }

    std::shared_ptr<GlIndexBuffer> GlVertexArray::get_index_buffer(std::size_t index) const {
        assert(m_index_buffers.size() > index);

        return m_index_buffers[index];
    }
}
