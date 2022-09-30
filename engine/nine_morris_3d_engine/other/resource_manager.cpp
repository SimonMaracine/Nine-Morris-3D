#include "nine_morris_3d_engine/other/resource_manager.h"

void Resources::merge(Resources& other) {
    texture.merge(std::move(other.texture));
    texture_3d.merge(std::move(other.texture_3d));
    vertex_array.merge(std::move(other.vertex_array));
    shader.merge(std::move(other.shader));
    buffer.merge(std::move(other.buffer));
    index_buffer.merge(std::move(other.index_buffer));
    uniform_buffer.merge(std::move(other.uniform_buffer));
    pixel_buffer.merge(std::move(other.pixel_buffer));
    framebuffer.merge(std::move(other.framebuffer));
    font.merge(std::move(other.font));
    material.merge(std::move(other.material));
    material_instance.merge(std::move(other.material_instance));
    texture_data.merge(std::move(other.texture_data));
    mesh_ptnt.merge(std::move(other.mesh_ptnt));
    mesh_ptn.merge(std::move(other.mesh_ptn));
    mesh_p.merge(std::move(other.mesh_p));
}
