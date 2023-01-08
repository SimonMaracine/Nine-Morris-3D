#include <glad/glad.h>

#include "engine/graphics/opengl/buffer.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"

namespace gl {
    Buffer::Buffer(size_t size, DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, static_cast<int>(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        DEB_DEBUG("Created GL buffer {}", buffer);
    }

    Buffer::Buffer(const void* data, size_t size, DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<int>(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        DEB_DEBUG("Created GL buffer {}", buffer);
    }

    Buffer::~Buffer() {
        glDeleteBuffers(1, &buffer);

        DEB_DEBUG("Deleted GL buffer {}", buffer);
    }

    void Buffer::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }

    void Buffer::unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Buffer::upload_data(const void* data, size_t size) {
        glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<int>(hint));
    }

    // --- Index buffer

    IndexBuffer::IndexBuffer(const unsigned int* data, size_t size) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        index_count = static_cast<int>(size / sizeof(unsigned int));

        DEB_DEBUG("Created GL index buffer {}", buffer);
    }

    IndexBuffer::~IndexBuffer() {
        glDeleteBuffers(1, &buffer);

        DEB_DEBUG("Deleted GL index buffer {}", buffer);
    }

    void IndexBuffer::bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    }

    void IndexBuffer::unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // --- Uniform buffer

    UniformBuffer::UniformBuffer() {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        DEB_DEBUG("Created GL uniform buffer {}", buffer);
    }

    UniformBuffer::~UniformBuffer() {
        glDeleteBuffers(1, &buffer);

        delete[] data;

        DEB_DEBUG("Deleted GL uniform buffer {}", buffer);
    }

    void UniformBuffer::bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    }

    void UniformBuffer::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBuffer::set(const void* field_data, size_t field_index) {
        ASSERT(configured, "Uniform buffer must be configured");
        ASSERT(data != nullptr, "Data must be allocated");

        memcpy(data + fields[field_index].offset, field_data, fields[field_index].size);
    }

    void UniformBuffer::upload_data() {
        ASSERT(data != nullptr && size > 0, "Data must be allocated");

        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    }

    // --- Pixel buffer

    PixelBuffer::PixelBuffer(size_t size) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer);
        glBufferData(GL_PIXEL_PACK_BUFFER, size, nullptr, GL_STREAM_READ);
        const float value = 0.0f;
        glClearBufferData(GL_PIXEL_PACK_BUFFER, GL_R32F, GL_RED, GL_FLOAT, &value);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        dummy_data = new char[size];
        memset(dummy_data, 0, size);

        DEB_DEBUG("Created GL pixel buffer {}", buffer);
    }

    PixelBuffer::~PixelBuffer() {
        glDeleteBuffers(1, &buffer);

        delete[] dummy_data;

        DEB_DEBUG("Deleted GL pixel buffer {}", buffer);
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
            DEB_ERROR("Could not map GL buffer {}", buffer);

            data = dummy_data;
        }
    }

    void PixelBuffer::unmap_data() {
        const GLboolean success = glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

        if (success == GL_FALSE) {
            DEB_ERROR("Memory mapped by GL buffer {} became corrupted while it was used", buffer);
        }
    }
}
