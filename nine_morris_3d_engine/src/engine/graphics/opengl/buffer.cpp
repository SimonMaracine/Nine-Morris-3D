#include <glad/glad.h>

#include "engine/application_base/platform.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"

namespace gl {
    VertexBuffer::VertexBuffer(DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", buffer);
    }

    VertexBuffer::VertexBuffer(size_t size, DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, static_cast<int>(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", buffer);
    }

    VertexBuffer::VertexBuffer(const void* data, size_t size, DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<int>(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", buffer);
    }

    VertexBuffer::~VertexBuffer() {
        glDeleteBuffers(1, &buffer);

        LOG_DEBUG("Deleted GL vertex buffer {}", buffer);
    }

    void VertexBuffer::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }

    void VertexBuffer::unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::upload_data(const void* data, size_t size) {
        glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<int>(hint));
    }

    void VertexBuffer::upload_sub_data(const void* data, size_t offset, size_t size) {
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    // --- Index buffer

    IndexBuffer::IndexBuffer(const unsigned int* data, size_t size) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        ASSERT(size % sizeof(unsigned int) == 0, "Data may be corrupted");

        index_count = static_cast<int>(size / sizeof(unsigned int));

        LOG_DEBUG("Created GL index buffer {}", buffer);
    }

    IndexBuffer::~IndexBuffer() {
        glDeleteBuffers(1, &buffer);

        LOG_DEBUG("Deleted GL index buffer {}", buffer);
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

        LOG_DEBUG("Created GL uniform buffer {}", buffer);
    }

    UniformBuffer::~UniformBuffer() {
        glDeleteBuffers(1, &buffer);

        delete[] data;

        LOG_DEBUG("Deleted GL uniform buffer {}", buffer);
    }

    void UniformBuffer::bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    }

    void UniformBuffer::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBuffer::set(const void* field_data, size_t field_index) {
        ASSERT(configured, "Uniform buffer must be configured");
        ASSERT(data != nullptr && size > 0, "Data must be allocated");

#ifdef NM3D_PLATFORM_DEBUG
        memcpy(data + fields.at(field_index).offset, field_data, fields.at(field_index).size);
#else
        memcpy(data + fields[field_index].offset, field_data, fields[field_index].size);
#endif
    }

    void UniformBuffer::upload_sub_data() {
        ASSERT(data != nullptr && size > 0, "Data must be allocated");

        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    }

    void UniformBuffer::allocate_memory(size_t size) {
        data = new char[size];
        this->size = size;

        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);
    }

    void UniformBuffer::add_field(size_t index, const UniformBlockField& field) {
        fields[index] = field;
    }

    void UniformBuffer::configure() {
        configured = true;
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

        LOG_DEBUG("Created GL pixel buffer {}", buffer);
    }

    PixelBuffer::~PixelBuffer() {
        glDeleteBuffers(1, &buffer);

        delete[] dummy_data;

        LOG_DEBUG("Deleted GL pixel buffer {}", buffer);
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
            LOG_ERROR("Could not map GL buffer {}", buffer);

            data = dummy_data;
        }
    }

    void PixelBuffer::unmap_data() {
        const GLboolean success = glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

        if (success == GL_FALSE) {
            LOG_ERROR("Memory mapped by GL buffer {} became corrupted while it was used", buffer);
        }
    }
}
