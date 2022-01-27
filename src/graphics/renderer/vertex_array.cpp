#include <memory>
#include <cstddef>
#include <cassert>

#include <glad/glad.h>

#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/buffer_layout.h"
#include "other/logging.h"

VertexArray::VertexArray(GLuint array)
    : array(array) {
    SPDLOG_DEBUG("Created vertex array {}", array);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &array);

    SPDLOG_DEBUG("Deleted vertex array {}", array);
}

std::shared_ptr<VertexArray> VertexArray::create() {
    GLuint array;
    glGenVertexArrays(1, &array);
    glBindVertexArray(array);

    return std::make_shared<VertexArray>(array);
}

void VertexArray::bind() const {
    glBindVertexArray(array);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::add_buffer(std::shared_ptr<Buffer> buffer, const BufferLayout& layout) {
    assert(layout.elements.size() > 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);

    size_t offset = 0;

    for (unsigned int i = 0; i < layout.elements.size(); i++) {
        const VertexElement& element = layout.elements[i];

        switch (element.type) {
            case GL_FLOAT:
                glVertexAttribPointer(element.index, element.size, element.type, GL_FALSE,
                                      layout.stride, (GLvoid*) offset);
                break;
            case GL_INT:
                glVertexAttribIPointer(element.index, element.size, element.type,
                                       layout.stride, (GLvoid*) offset);
                break;
            default:
                REL_CRITICAL("Unknown element type");
                std::exit(1);
        }

        glEnableVertexAttribArray(element.index);
        offset += element.size * VertexElement::get_size(element.type);
    }

    buffers.push_back(buffer);
}

void VertexArray::add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer);
    this->index_buffer = index_buffer;
}
