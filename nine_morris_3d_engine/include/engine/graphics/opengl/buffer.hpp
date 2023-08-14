#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>

#include <resmanager/resmanager.hpp>

namespace sm {
    enum class DrawHint {
        Static,
        Dynamic,
        Stream
    };

    class GlVertexBuffer {
    public:
        GlVertexBuffer(DrawHint hint = DrawHint::Static);
        GlVertexBuffer(std::size_t size, DrawHint hint = DrawHint::Static);
        GlVertexBuffer(const void* data, std::size_t size, DrawHint hint = DrawHint::Static);
        ~GlVertexBuffer();

        GlVertexBuffer(const GlVertexBuffer&) = delete;
        GlVertexBuffer& operator=(const GlVertexBuffer&) = delete;
        GlVertexBuffer(GlVertexBuffer&&) = delete;
        GlVertexBuffer& operator=(GlVertexBuffer&&) = delete;

        void bind();
        static void unbind();

        void upload_data(const void* data, std::size_t size);
        void upload_sub_data(const void* data, std::size_t offset, std::size_t size);

        unsigned int get_id() const { return buffer; }
    private:
        unsigned int buffer = 0;
        DrawHint hint = DrawHint::Static;
    };

    class GlIndexBuffer {
    public:
        GlIndexBuffer(const unsigned int* data, std::size_t size);
        ~GlIndexBuffer();

        GlIndexBuffer(const GlIndexBuffer&) = delete;
        GlIndexBuffer& operator=(const GlIndexBuffer&) = delete;
        GlIndexBuffer(GlIndexBuffer&&) = delete;
        GlIndexBuffer& operator=(GlIndexBuffer&&) = delete;

        void bind();
        static void unbind();

        unsigned int get_id() const { return buffer; }
        int get_index_count() { return index_count; }
    private:
        unsigned int buffer = 0;
        int index_count = 0;
    };

    struct UniformBlockSpecification {
        std::string block_name;
        std::vector<std::string> uniforms;
        unsigned int binding_index {};
    };

    class GlUniformBuffer {
    public:
        using Key = resmanager::HashedStr64;
        using KeyHash = resmanager::Hash<Key>;

        GlUniformBuffer(const UniformBlockSpecification& specification);
        ~GlUniformBuffer();

        GlUniformBuffer(const GlUniformBuffer&) = delete;
        GlUniformBuffer& operator=(const GlUniformBuffer&) = delete;
        GlUniformBuffer(GlUniformBuffer&&) = delete;
        GlUniformBuffer& operator=(GlUniformBuffer&&) = delete;

        void bind();
        static void unbind();

        bool is_configured() const { return configured; }
        void configure(unsigned int shader_program);

        void set(const void* field_data, Key field);
        void upload();
        void set_and_upload(const void* field_data, Key field);
    private:
        void allocate_memory(std::size_t size);
        static std::size_t type_size(unsigned int type);

        unsigned int buffer = 0;

        struct UniformBlockField {
            std::size_t offset = 0;
            std::size_t size = 0;
        };

        UniformBlockSpecification specification;

        unsigned char* data = nullptr;
        std::size_t size = 0;

        std::unordered_map<Key, UniformBlockField, KeyHash> fields;

        bool configured = false;

        friend class GlShader;
    };

    class GlPixelBuffer {
    public:
        GlPixelBuffer(std::size_t size);
        ~GlPixelBuffer();

        GlPixelBuffer(const GlPixelBuffer&) = delete;
        GlPixelBuffer& operator=(const GlPixelBuffer&) = delete;
        GlPixelBuffer(GlPixelBuffer&&) = delete;
        GlPixelBuffer& operator=(GlPixelBuffer&&) = delete;

        void bind();
        static void unbind();

        void map_data();
        void unmap_data();

        template<typename T>
        void get_data(T** data_out) {
            *data_out = static_cast<T*>(data);
        }
    private:
        unsigned int buffer = 0;
        void* data = nullptr;  // TODO clear value is float; should be generic
        unsigned char* dummy_data = nullptr;
    };
}
