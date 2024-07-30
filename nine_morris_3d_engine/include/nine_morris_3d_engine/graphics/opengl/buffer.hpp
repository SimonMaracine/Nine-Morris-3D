#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>
#include <string>

#include "nine_morris_3d_engine/application/id.hpp"

namespace sm {
    class GlShader;

    enum class DrawHint {
        Static,
        Dynamic,
        Stream
    };

    class GlVertexBuffer {
    public:
        explicit GlVertexBuffer(DrawHint hint = DrawHint::Static) noexcept;
        GlVertexBuffer(std::size_t size, DrawHint hint = DrawHint::Static) noexcept;
        GlVertexBuffer(const void* data, std::size_t size, DrawHint hint = DrawHint::Static) noexcept;
        ~GlVertexBuffer() noexcept;

        GlVertexBuffer(const GlVertexBuffer&) = delete;
        GlVertexBuffer& operator=(const GlVertexBuffer&) = delete;
        GlVertexBuffer(GlVertexBuffer&&) = delete;
        GlVertexBuffer& operator=(GlVertexBuffer&&) = delete;

        void bind() const noexcept;
        static void unbind() noexcept;

        void upload_data(const void* data, std::size_t size) const noexcept;
        void upload_sub_data(const void* data, std::size_t offset, std::size_t size) const noexcept;
    private:
        unsigned int m_buffer {};
        DrawHint m_hint {DrawHint::Static};
    };

    // Only supports unsigned int
    class GlIndexBuffer {
    public:
        GlIndexBuffer(const void* data, std::size_t size) noexcept;
        ~GlIndexBuffer() noexcept;

        GlIndexBuffer(const GlIndexBuffer&) = delete;
        GlIndexBuffer& operator=(const GlIndexBuffer&) = delete;
        GlIndexBuffer(GlIndexBuffer&&) = delete;
        GlIndexBuffer& operator=(GlIndexBuffer&&) = delete;

        void bind() const noexcept;
        static void unbind() noexcept;

        int get_index_count() const noexcept;
    private:
        unsigned int m_buffer {};
        int m_index_count {};
    };

    struct UniformBlockSpecification {
        std::string block_name;
        std::vector<std::string> uniforms;
        unsigned int binding_index {};
    };

    class GlUniformBuffer {
    public:
        explicit GlUniformBuffer(const UniformBlockSpecification& specification);
        ~GlUniformBuffer();

        GlUniformBuffer(const GlUniformBuffer&) = delete;
        GlUniformBuffer& operator=(const GlUniformBuffer&) = delete;
        GlUniformBuffer(GlUniformBuffer&&) = delete;
        GlUniformBuffer& operator=(GlUniformBuffer&&) = delete;

        void bind() const noexcept;
        static void unbind() noexcept;

        bool is_configured() const noexcept;
        void configure(unsigned int shader_program);

        void set(const void* field_data, Id field);
        void upload() const noexcept;
        void set_and_upload(const void* field_data, Id field);
    private:
        void allocate_memory(std::size_t size);
        static std::size_t type_size(unsigned int type) noexcept;

        unsigned int m_buffer {};

        struct UniformBlockField {
            std::size_t offset {};
            std::size_t size {};
        };

        UniformBlockSpecification m_specification;

        std::unordered_map<Id, UniformBlockField, Hash> m_fields;

        unsigned char* m_data {};
        std::size_t m_size {};

        bool m_configured {false};

        friend class GlShader;
    };

    class GlPixelBuffer {
    public:
        explicit GlPixelBuffer(std::size_t size);
        ~GlPixelBuffer() noexcept;

        GlPixelBuffer(const GlPixelBuffer&) = delete;
        GlPixelBuffer& operator=(const GlPixelBuffer&) = delete;
        GlPixelBuffer(GlPixelBuffer&&) = delete;
        GlPixelBuffer& operator=(GlPixelBuffer&&) = delete;

        void bind() const noexcept;
        static void unbind() noexcept;

        void map_data() noexcept;
        void unmap_data() const noexcept;

        template<typename T>
        void get_data(T** data_out) const noexcept {
            *data_out = static_cast<T*>(m_data);
        }
    private:
        unsigned int m_buffer {};
        void* m_data {};  // TODO clear value is float; should be generic
        unsigned char* m_dummy_data {};
    };
}
