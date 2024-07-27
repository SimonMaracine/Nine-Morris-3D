#pragma once

#include <mutex>

#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/audio/openal/source.hpp"
#include "nine_morris_3d_engine/audio/openal/buffer.hpp"
#include "nine_morris_3d_engine/audio/sound_data.hpp"
#include "nine_morris_3d_engine/audio/music_track.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"
#include "nine_morris_3d_engine/graphics/opengl/buffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"
#include "nine_morris_3d_engine/graphics/material.hpp"
#include "nine_morris_3d_engine/graphics/texture_data.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"

namespace sm::internal {
    template<typename T>
    class LockedCache {
    public:
        resmanager::Cache<T>& operator*() {
            std::lock_guard<std::mutex> lock {mutex};

            return cache;
        }

        resmanager::Cache<T>* operator->() {
            std::lock_guard<std::mutex> lock {mutex};

            return &cache;
        }
    private:
        resmanager::Cache<T> cache;
        std::mutex mutex;
    };

    // Global cache of resources
    struct ResourcesCache {
        void clear();

        LockedCache<Mesh> mesh;
        LockedCache<TextureData> texture_data;
        LockedCache<Font> font;
        LockedCache<Material> material;
        LockedCache<MaterialInstance> material_instance;
        LockedCache<GlTexture> texture;
        LockedCache<GlTextureCubemap> texture_cubemap;
        LockedCache<GlVertexArray> vertex_array;
        LockedCache<GlVertexBuffer> vertex_buffer;
        LockedCache<GlIndexBuffer> index_buffer;
        LockedCache<GlShader> shader;
        LockedCache<GlFramebuffer> framebuffer;
        LockedCache<SoundData> sound_data;
        LockedCache<MusicTrack> music_track;
        LockedCache<AlSource> source;
        LockedCache<AlBuffer> buffer;
    };
}
