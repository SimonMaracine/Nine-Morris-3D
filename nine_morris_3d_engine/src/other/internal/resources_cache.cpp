#include "nine_morris_3d_engine/other/internal/resources_cache.hpp"

namespace sm::internal {
    void ResourcesCache::clear() {
        texture->clear();
        texture_cubemap->clear();
        vertex_array->clear();
        shader->clear();
        vertex_buffer->clear();
        index_buffer->clear();
        // uniform_buffer.clear();
        // pixel_buffer.clear();
        framebuffer->clear();
        font->clear();
        material->clear();
        material_instance->clear();
        texture_data->clear();
        mesh->clear();
        // al_source.clear();
        // al_buffer.clear();
        // sound_data.clear();
        // music_track.clear();
    }
}
