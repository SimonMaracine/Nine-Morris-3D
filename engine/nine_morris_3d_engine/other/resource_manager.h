#pragma once

#include <resmanager/resmanager.h>

#include "nine_morris_3d_engine/graphics/renderer/opengl/texture.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d_engine/graphics/renderer/font.h"
#include "nine_morris_3d_engine/graphics/renderer/material.h"
#include "nine_morris_3d_engine/other/texture_data.h"
#include "nine_morris_3d_engine/other/mesh.h"

// struct

// TODO sound, music, sound data

using namespace mesh;

struct Resources {
    resmanager::Cache<Texture> texture;
    resmanager::Cache<Texture3D> texture_3d;
    resmanager::Cache<VertexArray> vertex_array;
    resmanager::Cache<Shader> shader;
    resmanager::Cache<Buffer> buffer;
    resmanager::Cache<IndexBuffer> index_buffer;
    resmanager::Cache<UniformBuffer> uniform_buffer;
    resmanager::Cache<PixelBuffer> pixel_buffer;
    resmanager::Cache<Framebuffer> framebuffer;
    resmanager::Cache<Font> font;
    resmanager::Cache<Material> material;
    resmanager::Cache<MaterialInstance> material_instance;
    resmanager::Cache<TextureData> texture_data;
    resmanager::Cache<Mesh<PTNT>> mesh_ptnt;
    resmanager::Cache<Mesh<PTN>> mesh_ptn;
    resmanager::Cache<Mesh<P>> mesh_p;
};
