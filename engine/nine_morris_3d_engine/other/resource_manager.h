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
    entt::resource_handle<VertexArray> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct ShaderLd : public entt::resource_loader<ShaderLd, Shader> {
    entt::resource_handle<Shader> load(const Sources& sources, const std::vector<std::string>& uniforms,
            const std::vector<UniformBlockSpecification>& uniform_blocks = {}) const {
        return std::make_shared<Shader>(sources, uniforms, uniform_blocks);
    }

    entt::resource_handle<Shader> load(const EncryptedSources& sources, const std::vector<std::string>& uniforms,
            const std::vector<UniformBlockSpecification>& uniform_blocks = {}) const {
        return std::make_shared<Shader>(sources, uniforms, uniform_blocks);
    }
};

struct BufferLd : public entt::resource_loader<BufferLd, Buffer> {
    entt::resource_handle<Buffer> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct IndexBufferLd : public entt::resource_loader<IndexBufferLd, IndexBuffer> {
    entt::resource_handle<IndexBuffer> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct UniformBufferLd : public entt::resource_loader<UniformBufferLd, UniformBuffer> {
    entt::resource_handle<UniformBuffer> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct PixelBufferLd : public entt::resource_loader<PixelBufferLd, PixelBuffer> {
    entt::resource_handle<PixelBuffer> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct FramebufferLd : public entt::resource_loader<FramebufferLd, Framebuffer> {
    entt::resource_handle<Framebuffer> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct FontLd : public entt::resource_loader<FontLd, Font> {
    entt::resource_handle<Font> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct MaterialLd : public entt::resource_loader<MaterialLd, Material> {
    entt::resource_handle<Material> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct MaterialInstanceLd : public entt::resource_loader<MaterialInstanceLd, MaterialInstance> {
    entt::resource_handle<MaterialInstance> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct TextureDataLd : public entt::resource_loader<TextureDataLd, TextureData> {
    entt::resource_handle<TextureData> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

using namespace mesh;

struct MeshVPTNTLd : public entt::resource_loader<MeshVPTNTLd, Mesh<VPTNT>> {
    entt::resource_handle<Mesh<VPTNT>> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct MeshVPTNLd : public entt::resource_loader<MeshVPTNLd, Mesh<VPTN>> {
    entt::resource_handle<Mesh<VPTN>> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
    }
};

struct MeshVPLd : public entt::resource_loader<MeshVPLd, Mesh<VP>> {
    entt::resource_handle<Mesh<VP>> load(int value) const {
        // return std::make_shared<Texture3D>();
        return {};
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
    entt::resource_cache<Mesh<VPTNT>> meshes_vptnt;
    entt::resource_cache<Mesh<VPTN>> meshes_vptn;
    entt::resource_cache<Mesh<VP>> meshes_vp;
};
