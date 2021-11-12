#include <memory>
#include <cstddef>
#include <cassert>

#include <glad/glad.h>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/buffer_layout.h"
#include "other/logging.h"

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

void VertexArray::bind() const {
    glBindVertexArray(array);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::add_buffer(std::shared_ptr<Buffer> buffer, const BufferLayout& layout) {
    assert(layout.elements.size() > 0);

    glBindVertexArray(array);  // TODO be careful with these
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);

    std::size_t offset = 0;

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
                spdlog::critical("Unknown element type");
                std::exit(1);
        }

        glEnableVertexAttribArray(element.index);
        offset += element.size * VertexElement::get_size(element.type);
    }

    buffers.push_back(buffer);
}

void VertexArray::hold_index_buffer(std::shared_ptr<Buffer> index_buffer) {  // TODO make this better
    buffers.push_back(index_buffer);
}
