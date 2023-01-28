#include "engine/other/resource_manager.h"

void ResourcesCache::merge(ResourcesCache& other) {
    texture.merge_replace(std::move(other.texture));
    texture_3d.merge_replace(std::move(other.texture_3d));
    vertex_array.merge_replace(std::move(other.vertex_array));
    shader.merge_replace(std::move(other.shader));
    vertex_buffer.merge_replace(std::move(other.vertex_buffer));
    index_buffer.merge_replace(std::move(other.index_buffer));
    uniform_buffer.merge_replace(std::move(other.uniform_buffer));
    pixel_buffer.merge_replace(std::move(other.pixel_buffer));
    framebuffer.merge_replace(std::move(other.framebuffer));
    font.merge_replace(std::move(other.font));
    material.merge_replace(std::move(other.material));
    material_instance.merge_replace(std::move(other.material_instance));
    texture_data.merge_replace(std::move(other.texture_data));
    mesh.merge_replace(std::move(other.mesh));
    al_source.merge_replace(std::move(other.al_source));
    al_buffer.merge_replace(std::move(other.al_buffer));
    sound_data.merge_replace(std::move(other.sound_data));
    music_track.merge_replace(std::move(other.music_track));
    model.merge_replace(std::move(other.model));
    quad.merge_replace(std::move(other.quad));
    image.merge_replace(std::move(other.image));
    text.merge_replace(std::move(other.text));
}
