#include "nine_morris_3d_engine/other/resources_cache.hpp"

namespace sm {
    void ResourcesCache::merge(ResourcesCache& other) {
        texture.merge(other.texture);
        texture_cubemap.merge(other.texture_cubemap);
        vertex_array.merge(other.vertex_array);
        // shader.merge(other.shader);
        // vertex_buffer.merge(other.vertex_buffer);
        // index_buffer.merge(other.index_buffer);
        // uniform_buffer.merge(other.uniform_buffer);
        // pixel_buffer.merge(other.pixel_buffer);
        // framebuffer.merge(other.framebuffer);
        font.merge(other.font);
        material.merge(other.material);
        material_instance.merge(other.material_instance);
        texture_data.merge(other.texture_data);
        mesh.merge(other.mesh);
        // al_source.merge(other.al_source);
        // al_buffer.merge(other.al_buffer);
        // sound_data.merge(other.sound_data);
        // music_track.merge(other.music_track);
    }

    void ResourcesCache::merge_replace(ResourcesCache& other) {
        texture.merge_replace(other.texture);
        texture_cubemap.merge_replace(other.texture_cubemap);
        vertex_array.merge_replace(other.vertex_array);
        // shader.merge_replace(other.shader);
        // vertex_buffer.merge_replace(other.vertex_buffer);
        // index_buffer.merge_replace(other.index_buffer);
        // uniform_buffer.merge_replace(other.uniform_buffer);
        // pixel_buffer.merge_replace(other.pixel_buffer);
        // framebuffer.merge_replace(other.framebuffer);
        font.merge_replace(other.font);
        material.merge_replace(other.material);
        material_instance.merge_replace(other.material_instance);
        texture_data.merge_replace(other.texture_data);
        mesh.merge_replace(other.mesh);
        // al_source.merge_replace(other.al_source);
        // al_buffer.merge_replace(other.al_buffer);
        // sound_data.merge_replace(other.sound_data);
        // music_track.merge_replace(other.music_track);
    }

    void ResourcesCache::clear() {
        texture.clear();
        texture_cubemap.clear();
        vertex_array.clear();
        // shader.clear();
        // vertex_buffer.clear();
        // index_buffer.clear();
        // uniform_buffer.clear();
        // pixel_buffer.clear();
        // framebuffer.clear();
        font.clear();
        material.clear();
        material_instance.clear();
        texture_data.clear();
        mesh.clear();
        // al_source.clear();
        // al_buffer.clear();
        // sound_data.clear();
        // music_track.clear();
    }
}
