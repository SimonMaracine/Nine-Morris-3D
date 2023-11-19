#include <memory>
#include <vector>
#include <cstddef>

#include <glad/glad.h>

#include "engine/application_base/panic.hpp"
#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/vertex_buffer_layout.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    GlVertexArray::GlVertexArray() {
        glGenVertexArrays(1, &array);
        glBindVertexArray(array);

        glBindVertexArray(0);

        LOG_DEBUG("Created GL vertex array {}", array);
    }

    GlVertexArray::~GlVertexArray() {
        glDeleteVertexArrays(1, &array);

        LOG_DEBUG("Deleted GL vertex array {}", array);
    }

    void GlVertexArray::bind() const {
        glBindVertexArray(array);
    }

    void GlVertexArray::unbind() {
        glBindVertexArray(0);
    }

    void GlVertexArray::add_vertex_buffer(std::shared_ptr<GlVertexBuffer> buffer, const VertexBufferLayout& layout) {
        SM_ASSERT(layout.elements.size() > 0, "Invalid layout");

        glBindBuffer(GL_ARRAY_BUFFER, buffer->get_id());

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
            offset += element.size * VertexBufferLayout::VertexElement::get_size(element.type);

            if (element.per_instance) {
                glVertexAttribDivisor(element.index, 1);
            }
        }

        vertex_buffers.push_back(buffer);
    }

    void GlVertexArray::add_index_buffer(std::shared_ptr<GlIndexBuffer> buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->get_id());

        index_buffer = buffer;
    }
}
