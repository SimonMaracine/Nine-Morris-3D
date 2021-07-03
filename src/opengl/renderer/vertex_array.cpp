#include <memory>
#include <cstddef>

#include <glad/glad.h>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/vertex_buffer.h"
#include "opengl/renderer/buffer_layout.h"
#include "logging.h"

VertexArray::VertexArray(GLuint array) : array(array) {
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

void VertexArray::bind() {
    glBindVertexArray(array);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::add_buffer(std::shared_ptr<VertexBuffer> buffer,
                             const BufferLayout& layout) {
    glBindVertexArray(array);  // TODO be careful with these
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);

    std::size_t offset = 0;

    for (unsigned int i = 0; i < layout.elements.size(); i++) {
        const VertexElement& element = layout.elements[i];

        glVertexAttribPointer(element.index, element.size, element.type, GL_FALSE,
                                layout.stride, (GLvoid*) offset);
        glEnableVertexAttribArray(element.index);
        offset += element.size * VertexElement::get_size(element.type);
    }
}
