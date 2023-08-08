#pragma once

#include <resmanager/resmanager.hpp>

#include "engine/audio/openal/source.h"
#include "engine/audio/openal/buffer.h"
#include "engine/audio/sound_data.h"
#include "engine/audio/music.h"
#include "engine/graphics/opengl/texture.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/graphics/font.h"
#include "engine/graphics/material.h"
#include "engine/graphics/texture_data.h"
#include "engine/other/mesh.h"

namespace sm {
    struct MeshLoader : public resmanager::DefaultLoader<mesh::Mesh> {
        struct PTN {};
        struct P {};
        struct PTNT {};

        template<typename... Args>
        std::shared_ptr<mesh::Mesh> load(PTN, Args&&... args) const {
            return mesh::load_model_PTN(std::forward<Args>(args)...);
        }

        template<typename... Args>
        std::shared_ptr<mesh::Mesh> load(P, Args&&... args) const {
            return mesh::load_model_P(std::forward<Args>(args)...);
        }

        template<typename... Args>
        std::shared_ptr<mesh::Mesh> load(PTNT, Args&&... args) const {
            return mesh::load_model_PTNT(std::forward<Args>(args)...);
        }
    };

    struct ResourcesCache final {
        void merge(ResourcesCache& other);

        resmanager::Cache<gl::Texture> texture;
        resmanager::Cache<gl::Texture3D> texture_3d;
        resmanager::Cache<gl::VertexArray> vertex_array;
        resmanager::Cache<gl::Shader> shader;
        resmanager::Cache<gl::VertexBuffer> vertex_buffer;
        resmanager::Cache<gl::IndexBuffer> index_buffer;
        resmanager::Cache<gl::UniformBuffer> uniform_buffer;
        resmanager::Cache<gl::PixelBuffer> pixel_buffer;
        resmanager::Cache<gl::Framebuffer> framebuffer;
        resmanager::Cache<Font> font;
        resmanager::Cache<Material> material;
        resmanager::Cache<MaterialInstance> material_instance;
        resmanager::Cache<TextureData> texture_data;
        resmanager::Cache<mesh::Mesh, MeshLoader> mesh;
        resmanager::Cache<al::Source> al_source;
        resmanager::Cache<al::Buffer> al_buffer;
        resmanager::Cache<SoundData> sound_data;
        resmanager::Cache<music::MusicTrack> music_track;
    };
}
