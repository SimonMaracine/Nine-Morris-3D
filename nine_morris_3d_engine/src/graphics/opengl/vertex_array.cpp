#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"

#include <cstddef>
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

        // Unbind index buffer after vertex array
        GlIndexBuffer::unbind();
    }

    void GlVertexArray::add_vertex_buffer(std::shared_ptr<GlVertexBuffer> buffer, const VertexBufferLayout& layout) {
        assert(layout.elements.size() > 0);

        buffer->bind();

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

        m_vertex_buffers.push_back(buffer);

        GlVertexBuffer::unbind();
    }

    void GlVertexArray::add_index_buffer(std::shared_ptr<GlIndexBuffer> buffer) {
        buffer->bind();

        m_index_buffer = buffer;
    }

    const GlIndexBuffer* GlVertexArray::get_index_buffer() const {
        return m_index_buffer.get();
    }
}
