#include "engine/other/resource_manager.hpp"

namespace sm {
    void ResourcesCache::merge(ResourcesCache& other) {
        texture.merge(other.texture);
        texture_3d.merge(other.texture_3d);
        vertex_array.merge(other.vertex_array);
        shader.merge(other.shader);
        vertex_buffer.merge(other.vertex_buffer);
        index_buffer.merge(other.index_buffer);
        uniform_buffer.merge(other.uniform_buffer);
        pixel_buffer.merge(other.pixel_buffer);
        framebuffer.merge(other.framebuffer);
        font.merge(other.font);
        material.merge(other.material);
        material_instance.merge(other.material_instance);
        texture_data.merge(other.texture_data);
        mesh.merge(other.mesh);
        al_source.merge(other.al_source);
        al_buffer.merge(other.al_buffer);
        sound_data.merge(other.sound_data);
        music_track.merge(other.music_track);
    }

    void ResourcesCache::merge_replace(ResourcesCache& other) {
        texture.merge_replace(other.texture);
        texture_3d.merge_replace(other.texture_3d);
        vertex_array.merge_replace(other.vertex_array);
        shader.merge_replace(other.shader);
        vertex_buffer.merge_replace(other.vertex_buffer);
        index_buffer.merge_replace(other.index_buffer);
        uniform_buffer.merge_replace(other.uniform_buffer);
        pixel_buffer.merge_replace(other.pixel_buffer);
        framebuffer.merge_replace(other.framebuffer);
        font.merge_replace(other.font);
        material.merge_replace(other.material);
        material_instance.merge_replace(other.material_instance);
        texture_data.merge_replace(other.texture_data);
        mesh.merge_replace(other.mesh);
        al_source.merge_replace(other.al_source);
        al_buffer.merge_replace(other.al_buffer);
        sound_data.merge_replace(other.sound_data);
        music_track.merge_replace(other.music_track);
    }
}
