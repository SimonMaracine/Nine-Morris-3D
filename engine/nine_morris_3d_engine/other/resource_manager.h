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

struct LTexture : public entt::resource_loader<LTexture, Texture> {
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

struct LTexture3D : public entt::resource_loader<LTexture3D, Texture3D> {
    entt::resource_handle<Texture3D> load(const char** file_paths) const {
        return std::make_shared<Texture3D>(file_paths);
    }

    entt::resource_handle<Texture3D> load(const std::array<entt::resource_handle<TextureData>, 6>& data) const {
        return std::make_shared<Texture3D>(data);
    }
};

struct LVertexArray : public entt::resource_loader<LVertexArray, VertexArray> {
    entt::resource_handle<VertexArray> load() const {
        return std::make_shared<VertexArray>();
    }
};

struct LShader : public entt::resource_loader<LShader, Shader> {
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

struct LBuffer : public entt::resource_loader<LBuffer, Buffer> {
    entt::resource_handle<Buffer> load(size_t size, DrawHint hint = DrawHint::Static) const {
        return std::make_shared<Buffer>(size, hint);
    }

    entt::resource_handle<Buffer> load(const void* data, size_t size, DrawHint hint = DrawHint::Static) const {
        return std::make_shared<Buffer>(data, size, hint);
    }
};

struct LIndexBuffer : public entt::resource_loader<LIndexBuffer, IndexBuffer> {
    entt::resource_handle<IndexBuffer> load(const unsigned int* data, size_t size) const {
        return std::make_shared<IndexBuffer>(data, size);
    }
};

struct LUniformBuffer : public entt::resource_loader<LUniformBuffer, UniformBuffer> {
    entt::resource_handle<UniformBuffer> load() const {
        return std::make_shared<UniformBuffer>();
    }
};

struct LPixelBuffer : public entt::resource_loader<LPixelBuffer, PixelBuffer> {
    entt::resource_handle<PixelBuffer> load(size_t size) const {
        return std::make_shared<PixelBuffer>(size);
    }
};

struct LFramebuffer : public entt::resource_loader<LFramebuffer, Framebuffer> {
    entt::resource_handle<Framebuffer> load(const FramebufferSpecification& specification) const {
        return std::make_shared<Framebuffer>(specification);
    }
};

struct LFont : public entt::resource_loader<LFont, Font> {
    entt::resource_handle<Font> load(
            std::string_view file_path, float size,
            int padding, unsigned char on_edge_value,
            int pixel_dist_scale, int bitmap_size) const {
        return std::make_shared<Font>(file_path, size, padding, on_edge_value, pixel_dist_scale, bitmap_size);
    }
};

struct LMaterial : public entt::resource_loader<LMaterial, Material> {
    entt::resource_handle<Material> load(entt::resource_handle<Shader> shader, int flags = 0) const {
        return std::make_shared<Material>(shader, flags);
    }
};

struct MaterialInstanceLd : public entt::resource_loader<MaterialInstanceLd, MaterialInstance> {
    entt::resource_handle<MaterialInstance> load(entt::resource_handle<Material> material) const {
        return std::make_shared<MaterialInstance>(material);
    }
};

struct LTextureData : public entt::resource_loader<LTextureData, TextureData> {
    entt::resource_handle<TextureData> load(std::string_view file_path, bool flip = false) const {
        return std::make_shared<TextureData>(file_path, flip);
    }

    entt::resource_handle<TextureData> load(encrypt::EncryptedFile file_path, bool flip = false) const {
        return std::make_shared<TextureData>(file_path, flip);
    }
};

using namespace mesh;

struct LMeshPTNT : public entt::resource_loader<LMeshPTNT, Mesh<PTNT>> {
    entt::resource_handle<Mesh<PTNT>> load(std::string_view file_path, bool flip_winding = false) const {
        return load_model_PTNT(file_path, flip_winding);
    }

    entt::resource_handle<Mesh<PTNT>> load(encrypt::EncryptedFile file_path, bool flip_winding = false) const {
        return load_model_PTNT(file_path, flip_winding);
    }
};

struct LMeshPTN : public entt::resource_loader<LMeshPTN, Mesh<PTN>> {
    entt::resource_handle<Mesh<PTN>> load(std::string_view file_path, bool flip_winding = false) const {
        return load_model_PTN(file_path, flip_winding);
    }

    entt::resource_handle<Mesh<PTN>> load(encrypt::EncryptedFile file_path, bool flip_winding = false) const {
        return load_model_PTN(file_path, flip_winding);
    }
};

struct LMeshP : public entt::resource_loader<LMeshP, Mesh<P>> {
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
