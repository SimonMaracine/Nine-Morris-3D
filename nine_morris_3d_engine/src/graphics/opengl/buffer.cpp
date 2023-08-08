#include <cstddef>

#include <glad/glad.h>

#include "engine/application_base/platform.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    static int draw_hint_to_int(DrawHint hint) {
        int result = 0;

        switch (hint) {
            case DrawHint::Static:
                result = GL_STATIC_DRAW;
                break;
            case DrawHint::Dynamic:
                result = GL_DYNAMIC_DRAW;
                break;
            case DrawHint::Stream:
                result = GL_STREAM_DRAW;
                break;
        }

        return result;
    }

    GlVertexBuffer::GlVertexBuffer(DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", buffer);
    }

    GlVertexBuffer::GlVertexBuffer(size_t size, DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, draw_hint_to_int(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", buffer);
    }

    GlVertexBuffer::GlVertexBuffer(const void* data, size_t size, DrawHint hint)
        : hint(hint) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, data, draw_hint_to_int(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", buffer);
    }

    GlVertexBuffer::~GlVertexBuffer() {
        glDeleteBuffers(1, &buffer);

        LOG_DEBUG("Deleted GL vertex buffer {}", buffer);
    }

    void GlVertexBuffer::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }

    void GlVertexBuffer::unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void GlVertexBuffer::upload_data(const void* data, size_t size) {
        glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<int>(hint));
    }

    void GlVertexBuffer::upload_sub_data(const void* data, size_t offset, size_t size) {
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    // --- Index buffer

    GlIndexBuffer::GlIndexBuffer(const unsigned int* data, size_t size) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        ASSERT(size % sizeof(unsigned int) == 0, "Data may be corrupted");

        index_count = static_cast<int>(size / sizeof(unsigned int));

        LOG_DEBUG("Created GL index buffer {}", buffer);
    }

    GlIndexBuffer::~GlIndexBuffer() {
        glDeleteBuffers(1, &buffer);

        LOG_DEBUG("Deleted GL index buffer {}", buffer);
    }

    void GlIndexBuffer::bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    }

    void GlIndexBuffer::unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // --- Uniform buffer

    GlUniformBuffer::GlUniformBuffer() {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        LOG_DEBUG("Created GL uniform buffer {}", buffer);
    }

    GlUniformBuffer::~GlUniformBuffer() {
        glDeleteBuffers(1, &buffer);

        delete[] data;

        LOG_DEBUG("Deleted GL uniform buffer {}", buffer);
    }

    void GlUniformBuffer::bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    }

    void GlUniformBuffer::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void GlUniformBuffer::set(const void* field_data, size_t field_index) {
        ASSERT(configured, "Uniform buffer must be configured");
        ASSERT(data != nullptr && size > 0, "Data must be allocated");

#ifdef SM_BUILD_DEBUG
        memcpy(data + fields.at(field_index).offset, field_data, fields.at(field_index).size);
#else
        memcpy(data + fields[field_index].offset, field_data, fields[field_index].size);
#endif
    }

    void GlUniformBuffer::upload_sub_data() {
        ASSERT(data != nullptr && size > 0, "Data must be allocated");

        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    }

    void GlUniformBuffer::allocate_memory(size_t size) {
        data = new char[size];
        this->size = size;

        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);
    }

    void GlUniformBuffer::add_field(size_t index, const UniformBlockField& field) {
        fields[index] = field;
    }

    void GlUniformBuffer::configure() {
        configured = true;
    }

    // --- Pixel buffer

    GlPixelBuffer::GlPixelBuffer(size_t size) {
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

    GlPixelBuffer::~GlPixelBuffer() {
        glDeleteBuffers(1, &buffer);

        delete[] dummy_data;

        LOG_DEBUG("Deleted GL pixel buffer {}", buffer);
    }

    void GlPixelBuffer::bind() {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer);
    }

    void GlPixelBuffer::unbind() {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    void GlPixelBuffer::map_data() {
        data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

        if (data == nullptr) {
            LOG_ERROR("Could not map GL buffer {}", buffer);

            data = dummy_data;
        }
    }

    void GlPixelBuffer::unmap_data() {
        const GLboolean success = glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

        if (success == GL_FALSE) {
            LOG_ERROR("Memory mapped by GL buffer {} became corrupted while it was used", buffer);
        }
    }
}
