#include <glad/glad.h>

#include "nine_morris_3d_engine/graphics/buffer_layout.h"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/opengl/buffer.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/exit.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &array);
    glBindVertexArray(array);

    DEB_DEBUG("Created vertex array {}", array);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &array);

    DEB_DEBUG("Deleted vertex array {}", array);
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
    }
}

void VertexArray::add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer);
}
