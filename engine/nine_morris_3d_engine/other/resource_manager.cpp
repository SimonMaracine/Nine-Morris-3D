#include "nine_morris_3d_engine/other/resource_manager.h"

void Resources::merge(Resources& other) {
    texture.merge_replace(std::move(other.texture));
    texture_3d.merge_replace(std::move(other.texture_3d));
    vertex_array.merge_replace(std::move(other.vertex_array));
    shader.merge_replace(std::move(other.shader));
    buffer.merge_replace(std::move(other.buffer));
    index_buffer.merge_replace(std::move(other.index_buffer));
    uniform_buffer.merge_replace(std::move(other.uniform_buffer));
    pixel_buffer.merge_replace(std::move(other.pixel_buffer));
    framebuffer.merge_replace(std::move(other.framebuffer));
    font.merge_replace(std::move(other.font));
    material.merge_replace(std::move(other.material));
    material_instance.merge_replace(std::move(other.material_instance));
    texture_data.merge_replace(std::move(other.texture_data));
    mesh_ptnt.merge_replace(std::move(other.mesh_ptnt));
    mesh_ptn.merge_replace(std::move(other.mesh_ptn));
    mesh_p.merge_replace(std::move(other.mesh_p));
    al_source.merge_replace(std::move(other.al_source));
    al_buffer.merge_replace(std::move(other.al_buffer));
    sound_data.merge_replace(std::move(other.sound_data));
}
