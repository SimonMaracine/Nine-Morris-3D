#pragma once

#include <entt/entt.hpp>

#include "nine_morris_3d_engine/graphics/renderer/opengl/texture.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d_engine/graphics/renderer/font.h"
#include "nine_morris_3d_engine/graphics/renderer/material.h"
#include "nine_morris_3d_engine/other/texture_data.h"
#include "nine_morris_3d_engine/other/mesh.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encrypt.h"

struct TextureLd : public entt::resource_loader<TextureLd, Texture> {
    entt::resource_handle<Texture> load(std::string_view file_path, 
            const TextureSpecification& specification) const {
        return std::make_shared<Texture>(file_path, specification);
    }

    entt::resource_handle<Texture> load(encrypt::EncryptedFile file_path,
            const TextureSpecification& specification) const {
        return std::make_shared<Texture>(file_path, specification);
    }

    entt::resource_handle<Texture> load(entt::resource_handle<TextureData> data,
            const TextureSpecification& specification) const {
        return std::make_shared<Texture>(data, specification);
    }
};

struct Texture3DLd : public entt::resource_loader<Texture3DLd, Texture3D> {
    entt::resource_handle<Texture3D> load(const char** file_paths) const {
        return std::make_shared<Texture3D>(file_paths);
    }

    entt::resource_handle<Texture3D> load(const std::array<entt::resource_handle<TextureData>, 6>& data) const {
        return std::make_shared<Texture3D>(data);
    }
};

struct VertexArrayLd : public entt::resource_loader<VertexArrayLd, VertexArray> {
    entt::resource_handle<VertexArray> load() const {
        return std::make_shared<VertexArray>();
    }
};

struct ShaderLd : public entt::resource_loader<ShaderLd, Shader> {
    entt::resource_handle<Shader> load(
            std::string_view vertex_source_path,
            std::string_view fragment_source_path,
            const std::vector<std::string>& uniforms,
            const std::vector<UniformBlockSpecification>& uniform_blocks = {}) const {
        return std::make_shared<Shader>(vertex_source_path, fragment_source_path, uniforms, uniform_blocks);
    }

    entt::resource_handle<Shader> load(
            encrypt::EncryptedFile vertex_source_path,
            encrypt::EncryptedFile fragment_source_path,
            const std::vector<std::string>& uniforms,
            const std::vector<UniformBlockSpecification>& uniform_blocks = {}) const {
        return std::make_shared<Shader>(vertex_source_path, fragment_source_path, uniforms, uniform_blocks);
    }
};

struct BufferLd : public entt::resource_loader<BufferLd, Buffer> {
    entt::resource_handle<Buffer> load(size_t size, DrawHint hint = DrawHint::Static) const {
        return std::make_shared<Buffer>(size, hint);
    }

    entt::resource_handle<Buffer> load(const void* data, size_t size, DrawHint hint = DrawHint::Static) const {
        return std::make_shared<Buffer>(data, size, hint);
    }
};

struct IndexBufferLd : public entt::resource_loader<IndexBufferLd, IndexBuffer> {
    entt::resource_handle<IndexBuffer> load(const unsigned int* data, size_t size) const {
        return std::make_shared<IndexBuffer>(data, size);
    }
};

struct UniformBufferLd : public entt::resource_loader<UniformBufferLd, UniformBuffer> {
    entt::resource_handle<UniformBuffer> load() const {
        return std::make_shared<UniformBuffer>();
    }
};

struct PixelBufferLd : public entt::resource_loader<PixelBufferLd, PixelBuffer> {
    entt::resource_handle<PixelBuffer> load(size_t size) const {
        return std::make_shared<PixelBuffer>(size);
    }
};

struct FramebufferLd : public entt::resource_loader<FramebufferLd, Framebuffer> {
    entt::resource_handle<Framebuffer> load(const FramebufferSpecification& specification) const {
        return std::make_shared<Framebuffer>(specification);
    }
};

struct FontLd : public entt::resource_loader<FontLd, Font> {
    entt::resource_handle<Font> load(
            std::string_view file_path, float size,
            int padding, unsigned char on_edge_value,
            int pixel_dist_scale, int bitmap_size) const {
        return std::make_shared<Font>(file_path, size, padding, on_edge_value, pixel_dist_scale, bitmap_size);
    }
};

struct MaterialLd : public entt::resource_loader<MaterialLd, Material> {
    entt::resource_handle<Material> load(entt::resource_handle<Shader> shader, int flags = 0) const {
        return std::make_shared<Material>(shader, flags);
    }
};

struct MaterialInstanceLd : public entt::resource_loader<MaterialInstanceLd, MaterialInstance> {
    entt::resource_handle<MaterialInstance> load(entt::resource_handle<Material> material) const {
        return std::make_shared<MaterialInstance>(material);
    }
};

struct TextureDataLd : public entt::resource_loader<TextureDataLd, TextureData> {
    entt::resource_handle<TextureData> load(std::string_view file_path, bool flip = false) const {
        return std::make_shared<TextureData>(file_path, flip);
    }

    entt::resource_handle<TextureData> load(encrypt::EncryptedFile file_path, bool flip = false) const {
        return std::make_shared<TextureData>(file_path, flip);
    }
};

using namespace mesh;

struct MeshPTNTLd : public entt::resource_loader<MeshPTNTLd, Mesh<PTNT>> {
    entt::resource_handle<Mesh<PTNT>> load(std::string_view file_path, bool flip_winding = false) const {
        return load_model_PTNT(file_path, flip_winding);
    }

    entt::resource_handle<Mesh<PTNT>> load(encrypt::EncryptedFile file_path, bool flip_winding = false) const {
        return load_model_PTNT(file_path, flip_winding);
    }
};

struct MeshPTNLd : public entt::resource_loader<MeshPTNLd, Mesh<PTN>> {
    entt::resource_handle<Mesh<PTN>> load(std::string_view file_path, bool flip_winding = false) const {
        return load_model_PTN(file_path, flip_winding);
    }

    entt::resource_handle<Mesh<PTN>> load(encrypt::EncryptedFile file_path, bool flip_winding = false) const {
        return load_model_PTN(file_path, flip_winding);
    }
};

struct MeshPLd : public entt::resource_loader<MeshPLd, Mesh<P>> {
    entt::resource_handle<Mesh<P>> load(std::string_view file_path, bool flip_winding = false) const {
        return load_model_P(file_path, flip_winding);
    }

    entt::resource_handle<Mesh<P>> load(encrypt::EncryptedFile file_path, bool flip_winding = false) const {
        return load_model_P(file_path, flip_winding);
    }
};

// TODO sound, music, sound data

struct ResourceManager {
    entt::resource_cache<Texture> textures;
    entt::resource_cache<Texture3D> textures_3d;
    entt::resource_cache<VertexArray> vertex_arrays;
    entt::resource_cache<Shader> shaders;
    entt::resource_cache<Buffer> buffers;
    entt::resource_cache<IndexBuffer> index_buffers;
    entt::resource_cache<UniformBuffer> uniform_buffers;
    entt::resource_cache<PixelBuffer> pixel_buffers;
    entt::resource_cache<Framebuffer> framebuffers;
    entt::resource_cache<Font> fonts;
    entt::resource_cache<Material> materials;
    entt::resource_cache<MaterialInstance> material_instances;
    entt::resource_cache<TextureData> texture_data;
    entt::resource_cache<Mesh<PTNT>> meshes_ptnt;
    entt::resource_cache<Mesh<PTN>> meshes_ptn;
    entt::resource_cache<Mesh<P>> meshes_p;
};
