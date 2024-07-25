#include "nine_morris_3d_engine/other/internal/resources_cache.hpp"

namespace sm::internal {
    void ResourcesCache::clear() {
        mesh->clear();
        texture_data->clear();
        font->clear();
        material->clear();
        material_instance->clear();
        texture->clear();
        texture_cubemap->clear();
        vertex_array->clear();
        vertex_buffer->clear();
        index_buffer->clear();
        shader->clear();
        framebuffer->clear();
        sound_data->clear();
        music_track->clear();
        source->clear();
        buffer->clear();
    }
}
