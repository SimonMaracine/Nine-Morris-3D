#include <memory>
#include <cstddef>
#include <string.h>

#include <glad/glad.h>

#include "graphics/renderer/buffer.h"
#include "graphics/debug_opengl.h"
#include "other/logging.h"

Buffer::Buffer(GLuint buffer)
    : buffer(buffer) {
    DEB_DEBUG("Created buffer {}", buffer);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &buffer);

    DEB_DEBUG("Deleted buffer {}", buffer);
}

std::shared_ptr<Buffer> Buffer::create(size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    LOG_ALLOCATION(size);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return std::make_shared<Buffer>(buffer);
}

std::shared_ptr<Buffer> Buffer::create(const void* data, size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return std::make_shared<Buffer>(buffer);
}

void Buffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void Buffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffer::update_data(const void* data, size_t size) {
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

// --- Index buffer

IndexBuffer::IndexBuffer(GLuint buffer)
    : buffer(buffer) {
    DEB_DEBUG("Created index buffer {}", buffer);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &buffer);

    DEB_DEBUG("Deleted index buffer {}", buffer);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(const unsigned int* data, size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return std::make_shared<IndexBuffer>(buffer);
}

void IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// --- Uniform buffer

UniformBuffer::UniformBuffer(GLuint buffer)
    : buffer(buffer) {
    DEB_DEBUG("Created uniform buffer {}", buffer);
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &buffer);

    delete[] data;

    DEB_DEBUG("Deleted uniform buffer {}", buffer);
}

std::shared_ptr<UniformBuffer> UniformBuffer::create() {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return std::make_shared<UniformBuffer>(buffer);
}

void UniformBuffer::bind() {
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
}

void UniformBuffer::unbind() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::set(const void* field_data, unsigned field_index) {
    memcpy(data + fields[field_index].offset, field_data, fields[field_index].size);
}

void UniformBuffer::upload_data() {
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);  // TODO see what is best
}

// --- Pixel buffer

PixelBuffer::PixelBuffer(GLuint buffer, size_t size)
    : buffer(buffer) {
    dummy_data = new char[size];
    memset(dummy_data, 0, size);

    DEB_DEBUG("Created pixel buffer {}", buffer);
}

PixelBuffer::~PixelBuffer() {
    glDeleteBuffers(1, &buffer);

    delete[] dummy_data;

    DEB_DEBUG("Deleted pixel buffer {}", buffer);
}

std::shared_ptr<PixelBuffer> PixelBuffer::create(size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer);
    glBufferData(GL_PIXEL_PACK_BUFFER, size, nullptr, GL_STREAM_READ);
    int value = 0;
    glClearBufferData(GL_PIXEL_PACK_BUFFER, GL_R32I, GL_RED_INTEGER, GL_INT, &value);
    LOG_ALLOCATION(size);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    return std::make_shared<PixelBuffer>(buffer, size);
}

void PixelBuffer::bind() {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer);
}

void PixelBuffer::unbind() {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void PixelBuffer::map_data() {
    data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (data == nullptr) {
        DEB_ERROR("Could not map buffer {}", buffer);

        data = dummy_data;
    }
}

void PixelBuffer::unmap_data() {
    GLboolean success = glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    if (success == GL_FALSE) {
        DEB_ERROR("Memory mapped by buffer {} became corrupted while it was used", buffer);
    }
}
