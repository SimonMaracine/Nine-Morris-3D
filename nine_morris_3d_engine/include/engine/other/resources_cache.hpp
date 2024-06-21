#pragma once

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
#include "engine/graphics/renderer.hpp"
#include "engine/graphics/font.hpp"
#include "engine/graphics/material.hpp"
#include "engine/graphics/texture_data.hpp"
#include "engine/graphics/mesh.hpp"

namespace sm {
    class Application;
    class Ctx;

    class ResourcesCache {
    public:
        // resmanager::Cache<GlTexture> texture;
        // resmanager::Cache<GlTextureCubemap> texture_3d;
        resmanager::Cache<GlVertexArray> vertex_array;
        // resmanager::Cache<GlShader> shader;
        // resmanager::Cache<GlVertexBuffer> vertex_buffer;
        // resmanager::Cache<GlIndexBuffer> index_buffer;
        // resmanager::Cache<GlUniformBuffer> uniform_buffer;
        // resmanager::Cache<GlPixelBuffer> pixel_buffer;
        // resmanager::Cache<GlFramebuffer> framebuffer;
        resmanager::Cache<Font> font;
        resmanager::Cache<Material> material;
        resmanager::Cache<MaterialInstance> material_instance;
        // resmanager::Cache<TextureData> texture_data;
        // resmanager::Cache<Mesh> mesh;
        // resmanager::Cache<AlSource> al_source;
        // resmanager::Cache<AlBuffer> al_buffer;
        // resmanager::Cache<SoundData> sound_data;
        // resmanager::Cache<MusicTrack> music_track;
    private:
        ResourcesCache() = default;

        void merge(ResourcesCache& other);
        void merge_replace(ResourcesCache& other);
        void clear();

        friend class Application;
        friend class Ctx;
    };
}
