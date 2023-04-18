#include <glad/glad.h>

#include "engine/graphics/vertex_buffer_layout.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/application/panic.h"

namespace gl {
    VertexArray::VertexArray() {
        glGenVertexArrays(1, &array);
        glBindVertexArray(array);

        glBindVertexArray(0);

        LOG_DEBUG("Created GL vertex array {}", array);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &array);

        LOG_DEBUG("Deleted GL vertex array {}", array);
    }

    void VertexArray::bind() {
        glBindVertexArray(array);
    }

    void VertexArray::unbind() {
        glBindVertexArray(0);
    }

    VertexArray::Def VertexArray::begin_definition() {
        glBindVertexArray(array);

        return Def {};
    }

    VertexArray::Def& VertexArray::Def::add_buffer(std::shared_ptr<VertexBuffer> buffer, const VertexBufferLayout& layout) {
        ASSERT(layout.elements.size() > 0, "Invalid layout");

        glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);

        size_t offset = 0;

        for (size_t i = 0; i < layout.elements.size(); i++) {
            const VertexBufferLayout::VertexElement& element = layout.elements[i];

            switch (element.type) {
                case VertexBufferLayout::Float:
                    glVertexAttribPointer(
                        element.index, element.size, GL_FLOAT, GL_FALSE,
                        layout.stride, reinterpret_cast<GLvoid*>(offset)
                    );
                    break;
                case VertexBufferLayout::Int:
                    glVertexAttribIPointer(
                        element.index, element.size, GL_INT,
                        layout.stride, reinterpret_cast<GLvoid*>(offset)
                    );
                    break;
                default:
                    LOG_DIST_CRITICAL("Unknown element type `{}`", element.type);
                    panic::panic();
            }

            glEnableVertexAttribArray(element.index);
            offset += element.size * VertexBufferLayout::VertexElement::get_size(element.type);

            if (element.per_instance) {
                glVertexAttribDivisor(element.index, 1);
            }
        }

        return *this;
    }

    VertexArray::Def& VertexArray::Def::add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer);

        return *this;
    }

    void VertexArray::Def::end_definition() {
        glBindVertexArray(0);
    }
}
