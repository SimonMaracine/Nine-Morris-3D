#pragma once

#include <utility>
#include <memory>

#include <resmanager/resmanager.hpp>

#include "engine/audio/openal/source.hpp"
#include "engine/audio/openal/buffer.hpp"
#include "engine/audio/sound_data.hpp"
#include "engine/audio/music.hpp"
#include "engine/graphics/opengl/texture.hpp"
#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/renderer/renderer.hpp"
#include "engine/graphics/renderer/gui_renderer.hpp"
#include "engine/graphics/font.hpp"
#include "engine/graphics/material.hpp"
#include "engine/graphics/texture_data.hpp"
#include "engine/other/mesh.hpp"

namespace sm {
    struct MeshLoader : public resmanager::DefaultLoader<Mesh> {
        struct PTN {};
        struct P {};
        struct PTNT {};

        template<typename... Args>
        std::shared_ptr<Mesh> load(PTN, Args&&... args) const {
            return load_model_PTN(std::forward<Args>(args)...);
        }

        template<typename... Args>
        std::shared_ptr<Mesh> load(P, Args&&... args) const {
            return load_model_P(std::forward<Args>(args)...);
        }

        template<typename... Args>
        std::shared_ptr<Mesh> load(PTNT, Args&&... args) const {
            return load_model_PTNT(std::forward<Args>(args)...);
        }
    };

    struct ResourcesCache {
        resmanager::Cache<GlTexture> texture;
        resmanager::Cache<GlTexture3D> texture_3d;
        resmanager::Cache<GlVertexArray> vertex_array;
        resmanager::Cache<GlShader> shader;
        resmanager::Cache<GlVertexBuffer> vertex_buffer;
        resmanager::Cache<GlIndexBuffer> index_buffer;
        resmanager::Cache<GlUniformBuffer> uniform_buffer;
        resmanager::Cache<GlPixelBuffer> pixel_buffer;
        resmanager::Cache<GlFramebuffer> framebuffer;
        resmanager::Cache<Font> font;
        resmanager::Cache<Material> material;
        resmanager::Cache<MaterialInstance> material_instance;
        resmanager::Cache<TextureData> texture_data;
        resmanager::Cache<Mesh, MeshLoader> mesh;
        resmanager::Cache<AlSource> al_source;
        resmanager::Cache<AlBuffer> al_buffer;
        resmanager::Cache<SoundData> sound_data;
        resmanager::Cache<MusicTrack> music_track;

        void merge(ResourcesCache& other);
    };
}
