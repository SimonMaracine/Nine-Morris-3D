#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "nine_morris_3d_engine/application/id.hpp"

// OpenGL resources representing buffers

namespace sm {
    class GlShader;

    enum class DrawHint {
        Static,
        Dynamic,
        Stream
    };

    class GlVertexBuffer {
    public:
        explicit GlVertexBuffer(DrawHint hint = DrawHint::Static);
        GlVertexBuffer(std::size_t size, DrawHint hint = DrawHint::Static);
        GlVertexBuffer(const void* data, std::size_t size, DrawHint hint = DrawHint::Static);
        ~GlVertexBuffer();

        GlVertexBuffer(const GlVertexBuffer&) = delete;
        GlVertexBuffer& operator=(const GlVertexBuffer&) = delete;
        GlVertexBuffer(GlVertexBuffer&&) = delete;
        GlVertexBuffer& operator=(GlVertexBuffer&&) = delete;

        void bind() const;
        static void unbind();

        void upload_data(const void* data, std::size_t size) const;
        void upload_sub_data(const void* data, std::size_t offset, std::size_t size) const;
    private:
        unsigned int m_buffer {};
        DrawHint m_hint {DrawHint::Static};
    };

    // Only supports unsigned int
    class GlIndexBuffer {
    public:
        GlIndexBuffer(const void* data, std::size_t size);
        ~GlIndexBuffer();

        GlIndexBuffer(const GlIndexBuffer&) = delete;
        GlIndexBuffer& operator=(const GlIndexBuffer&) = delete;
        GlIndexBuffer(GlIndexBuffer&&) = delete;
        GlIndexBuffer& operator=(GlIndexBuffer&&) = delete;

        void bind() const;
        static void unbind();

        int get_index_count() const;
    private:
        unsigned int m_buffer {};
        int m_index_count {};
    };

    // Used by shader
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

        void bind() const;
        static void unbind();

        bool is_configured() const;
        void configure(unsigned int shader_program);

        void set(const void* field_data, Id field);
        void upload() const;
        void set_and_upload(const void* field_data, Id field);
    private:
        void allocate_memory(std::size_t size);
        static std::size_t type_size(unsigned int type);

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
        ~GlPixelBuffer();

        GlPixelBuffer(const GlPixelBuffer&) = delete;
        GlPixelBuffer& operator=(const GlPixelBuffer&) = delete;
        GlPixelBuffer(GlPixelBuffer&&) = delete;
        GlPixelBuffer& operator=(GlPixelBuffer&&) = delete;

        void bind() const;
        static void unbind();

        void map_data();
        void unmap_data() const;

        template<typename T>
        void get_data(T** data_out) const {
            *data_out = static_cast<T*>(m_data);
        }
    private:
        unsigned int m_buffer {};
        void* m_data {};  // TODO clear value is float; should be generic
        unsigned char* m_dummy_data {};
    };
}
