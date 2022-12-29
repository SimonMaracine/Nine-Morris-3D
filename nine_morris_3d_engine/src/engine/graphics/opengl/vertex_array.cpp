#include <glad/glad.h>

#include "engine/graphics/buffer_layout.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/exit.h"

namespace gl {
    VertexArray::VertexArray() {
        glGenVertexArrays(1, &array);
        glBindVertexArray(array);

        DEB_DEBUG("Created GL vertex array {}", array);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &array);

        DEB_DEBUG("Deleted GL vertex array {}", array);
    }

    void VertexArray::bind() {
        glBindVertexArray(array);
    }

    void VertexArray::unbind() {
        glBindVertexArray(0);
    }

    void VertexArray::add_buffer(std::shared_ptr<Buffer> buffer, const BufferLayout& layout) {
        ASSERT(layout.elements.size() > 0, "Invalid layout");

        glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);

        size_t offset = 0;

        for (size_t i = 0; i < layout.elements.size(); i++) {
            const BufferLayout::VertexElement& element = layout.elements[i];

            switch (element.type) {
                case BufferLayout::Float:
                    glVertexAttribPointer(
                        element.index, element.size, GL_FLOAT, GL_FALSE,
                        layout.stride, reinterpret_cast<GLvoid*>(offset)
                    );
                    break;
                case BufferLayout::Int:
                    glVertexAttribIPointer(
                        element.index, element.size, GL_INT,
                        layout.stride, reinterpret_cast<GLvoid*>(offset)
                    );
                    break;
                default:
                    REL_CRITICAL("Unknown element type `{}`, exiting...", element.type);
                    game_exit::exit_critical();
            }

            glEnableVertexAttribArray(element.index);
            offset += element.size * BufferLayout::VertexElement::get_size(element.type);

            if (element.per_instance) {
                glVertexAttribDivisor(element.index, 1);
            }
        }
    }

    void VertexArray::add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer);
    }
}
