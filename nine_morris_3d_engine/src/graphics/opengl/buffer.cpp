#include "nine_morris_3d_engine/graphics/opengl/buffer.hpp"

#include <cstring>
#include <cassert>

#include <glad/glad.h>

#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    static int draw_hint_to_int(DrawHint hint) {
        int result {};

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
        : m_hint(hint) {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", m_buffer);
    }

    GlVertexBuffer::GlVertexBuffer(std::size_t size, DrawHint hint)
        : m_hint(hint) {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, draw_hint_to_int(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", m_buffer);
    }

    GlVertexBuffer::GlVertexBuffer(const void* data, std::size_t size, DrawHint hint)
        : m_hint(hint) {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBufferData(GL_ARRAY_BUFFER, size, data, draw_hint_to_int(hint));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        LOG_DEBUG("Created GL vertex buffer {}", m_buffer);
    }

    GlVertexBuffer::~GlVertexBuffer() {
        glDeleteBuffers(1, &m_buffer);

        LOG_DEBUG("Deleted GL vertex buffer {}", m_buffer);
    }

    void GlVertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    }

    void GlVertexBuffer::unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void GlVertexBuffer::upload_data(const void* data, std::size_t size) const {
        glBufferData(GL_ARRAY_BUFFER, size, data, draw_hint_to_int(m_hint));
    }

    void GlVertexBuffer::upload_sub_data(const void* data, std::size_t offset, std::size_t size) const {
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    GlIndexBuffer::GlIndexBuffer(const void* data, std::size_t size) {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        assert(size % sizeof(unsigned int) == 0);

        m_index_count = static_cast<int>(size / sizeof(unsigned int));

        LOG_DEBUG("Created GL index buffer {}", m_buffer);
    }

    GlIndexBuffer::~GlIndexBuffer() {
        glDeleteBuffers(1, &m_buffer);

        LOG_DEBUG("Deleted GL index buffer {}", m_buffer);
    }

    void GlIndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
    }

    void GlIndexBuffer::unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    int GlIndexBuffer::get_index_count() const {
        return m_index_count;
    }

    GlUniformBuffer::GlUniformBuffer(const UniformBlockSpecification& specification)
        : m_specification(specification) {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        LOG_DEBUG("Created GL uniform buffer {}", m_buffer);
    }

    GlUniformBuffer::~GlUniformBuffer() {
        glDeleteBuffers(1, &m_buffer);

        delete[] m_data;

        LOG_DEBUG("Deleted GL uniform buffer {}", m_buffer);
    }

    void GlUniformBuffer::bind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    }

    void GlUniformBuffer::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    bool GlUniformBuffer::is_configured() const {
        return m_configured;
    }

    void GlUniformBuffer::configure(unsigned int shader_program) {
        const unsigned int block_index {
            glGetUniformBlockIndex(shader_program, m_specification.block_name.c_str())
        };

        assert(block_index != GL_INVALID_INDEX);

        // If it's already configured, return
        if (m_configured) {
            return;
        }

        // Get data block size
        int block_size;
        glGetActiveUniformBlockiv(shader_program, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);

        // Allocate memory on both CPU and GPU side
        allocate_memory(block_size);

        // Link uniform buffer to binding index
        glBindBufferBase(GL_UNIFORM_BUFFER, m_specification.binding_index, m_buffer);

        const std::size_t field_count {m_specification.uniforms.size()};
        static constexpr std::size_t MAX_FIELD_COUNT {24};

        assert(field_count <= MAX_FIELD_COUNT);

        unsigned int indices[MAX_FIELD_COUNT];
        int offsets[MAX_FIELD_COUNT];
        int sizes[MAX_FIELD_COUNT];
        int types[MAX_FIELD_COUNT];

        // Create the uniforms names list; the order of these names matters
        const char* field_names[MAX_FIELD_COUNT];

        for (std::size_t i {0}; i < field_count; i++) {
            field_names[i] = m_specification.uniforms[i].c_str();
        }

        // Get uniform indices just to later get offsets, sizes and types
        glGetUniformIndices(
            shader_program,
            static_cast<int>(field_count),
            static_cast<const char* const*>(field_names),
            indices
        );

        for (std::size_t i {0}; i < field_count; i++) {
            assert(indices[i] != GL_INVALID_INDEX);
        }

        glGetActiveUniformsiv(shader_program, static_cast<int>(field_count), indices, GL_UNIFORM_OFFSET, offsets);
        glGetActiveUniformsiv(shader_program, static_cast<int>(field_count), indices, GL_UNIFORM_SIZE, sizes);  // For arrays
        glGetActiveUniformsiv(shader_program, static_cast<int>(field_count), indices, GL_UNIFORM_TYPE, types);

        // Finally setup the uniform block fields
        for (std::size_t i {0}; i < field_count; i++) {
            UniformBlockField field;
            field.offset = static_cast<std::size_t>(offsets[i]);
            field.size = static_cast<std::size_t>(sizes[i]) * type_size(types[i]);

            m_fields[Id(field_names[i])] = field;
        }

        m_configured = true;
    }

    void GlUniformBuffer::set(const void* field_data, Id field) {
        assert(m_configured);
        assert(m_data != nullptr && m_size > 0);

        std::memcpy(m_data + m_fields.at(field).offset, field_data, m_fields.at(field).size);
    }

    void GlUniformBuffer::upload() const {
        assert(m_data != nullptr && m_size > 0);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, m_size, m_data);
    }

    void GlUniformBuffer::set_and_upload(const void* field_data, Id field) {
        assert(m_configured);
        assert(m_data != nullptr && m_size > 0);

        const std::size_t offset {m_fields.at(field).offset};
        const std::size_t size {m_fields.at(field).size};

        std::memcpy(m_data + offset, field_data, size);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, m_data);
    }

    void GlUniformBuffer::allocate_memory(std::size_t size) {
        m_data = new unsigned char[size];
        m_size = size;

        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);
    }

    std::size_t GlUniformBuffer::type_size(unsigned int type) {
        std::size_t size {0};

        switch (type) {
            case GL_FLOAT_VEC3:
                size = 3 * sizeof(float);
                break;
            case GL_FLOAT:
                size = 1 * sizeof(float);
                break;
            case GL_FLOAT_MAT4:
                size = 16 * sizeof(float);
                break;
            default:
                assert(false);
                break;
        }

        return size;
    }

    GlPixelBuffer::GlPixelBuffer(std::size_t size) {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
        glBufferData(GL_PIXEL_PACK_BUFFER, size, nullptr, GL_STREAM_READ);
        const float value {0.0f};
        glClearBufferData(GL_PIXEL_PACK_BUFFER, GL_R32F, GL_RED, GL_FLOAT, &value);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        m_dummy_data = new unsigned char[size];
        std::memset(m_dummy_data, 0, size);

        LOG_DEBUG("Created GL pixel buffer {}", m_buffer);
    }

    GlPixelBuffer::~GlPixelBuffer() {
        glDeleteBuffers(1, &m_buffer);

        delete[] m_dummy_data;

        LOG_DEBUG("Deleted GL pixel buffer {}", m_buffer);
    }

    void GlPixelBuffer::bind() const {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
    }

    void GlPixelBuffer::unbind() {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    void GlPixelBuffer::map_data() {
        m_data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

        if (m_data == nullptr) {
            LOG_ERROR("Could not map GL pixel buffer {}", m_buffer);

            m_data = m_dummy_data;
        }
    }

    void GlPixelBuffer::unmap_data() const {
        const auto success {glUnmapBuffer(GL_PIXEL_PACK_BUFFER)};

        if (success == GL_FALSE) {
            LOG_ERROR("Memory mapped by GL buffer {} became corrupted while it was used", m_buffer);
        }
    }
}
