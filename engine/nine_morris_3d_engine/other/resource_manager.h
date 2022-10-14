#pragma once

#include <resmanager/resmanager.h>

#include "nine_morris_3d_engine/graphics/opengl/texture.h"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/opengl/buffer.h"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.h"
#include "nine_morris_3d_engine/graphics/font.h"
#include "nine_morris_3d_engine/graphics/material.h"
#include "nine_morris_3d_engine/other/texture_data.h"
#include "nine_morris_3d_engine/other/mesh.h"

// TODO sound, music, sound data

using namespace mesh;

struct _MeshPTNTLoader : public resmanager::Loader<Mesh<PTNT>> {
    template<typename... Args>
    std::shared_ptr<Mesh<PTNT>> load(Args&&... args) const {
        return load_model_PTNT(std::forward<Args>(args)...);
    }
};

struct _MeshPTNLoader : public resmanager::Loader<Mesh<PTN>> {
    template<typename... Args>
    std::shared_ptr<Mesh<PTN>> load(Args&&... args) const {
        return load_model_PTN(std::forward<Args>(args)...);
    }
};

struct _MeshPLoader : public resmanager::Loader<Mesh<P>> {
    template<typename... Args>
    std::shared_ptr<Mesh<P>> load(Args&&... args) const {
        return load_model_P(std::forward<Args>(args)...);
    }
};

struct Resources {
    void merge(Resources& other);

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
    resmanager::Cache<Mesh<PTNT>, _MeshPTNTLoader> mesh_ptnt;
    resmanager::Cache<Mesh<PTN>, _MeshPTNLoader> mesh_ptn;
    resmanager::Cache<Mesh<P>, _MeshPLoader> mesh_p;
};
