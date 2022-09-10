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

// TODO sound, music, sound data

using namespace mesh;

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
