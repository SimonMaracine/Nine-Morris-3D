#include <cassert>
#include <memory>

#include "other/asset_manager.h"
#include "other/logging.h"

void AssetManager::define_asset(unsigned int id, const std::string& file_path) {
    assert(all_assets.find(id) == all_assets.end());

    all_assets[id] = file_path;
}

void AssetManager::hold(unsigned int id, Rc<Mesh<FullVertex>> mesh) {
    CHECK_IDENTIFIER(id);

    full_meshes[id] = mesh;

    SPDLOG_DEBUG("Hold {}", id);
}

void AssetManager::hold(unsigned int id, Rc<Mesh<PositionVertex>> mesh) {
    CHECK_IDENTIFIER(id);

    position_meshes[id] = mesh;

    SPDLOG_DEBUG("Hold {}", id);
}

void AssetManager::hold(unsigned int id, Rc<VertexArray> vertex_array) {
    CHECK_IDENTIFIER(id);

    vertex_arrays[id] = vertex_array;

    SPDLOG_DEBUG("Hold {}", id);
}

void AssetManager::hold(unsigned int id, Rc<TextureData> texture_data) {
    CHECK_IDENTIFIER(id);

    texture_datas[id] = texture_data;

    SPDLOG_DEBUG("Hold {}", id);
}

void AssetManager::hold(unsigned int id, Rc<Texture> texture) {
    CHECK_IDENTIFIER(id);

    textures[id] = texture;

    SPDLOG_DEBUG("Hold {}", id);
}

void AssetManager::hold(unsigned int id, Rc<Texture3D> texture_3d) {
    CHECK_IDENTIFIER(id);

    textures_3d[id] = texture_3d;

    SPDLOG_DEBUG("Hold {}", id);
}

void AssetManager::drop(unsigned int id) {
    if (full_meshes.find(id) != full_meshes.end()) {
        full_meshes.erase(id);
    }

    if (position_meshes.find(id) != position_meshes.end()) {
        position_meshes.erase(id);
    }

    if (vertex_arrays.find(id) != vertex_arrays.end()) {
        vertex_arrays.erase(id);
    }

    if (texture_datas.find(id) != texture_datas.end()) {
        texture_datas.erase(id);
    }

    if (textures.find(id) != textures.end()) {
        textures.erase(id);
    }

    if (textures_3d.find(id) != textures_3d.end()) {
        textures_3d.erase(id);
    }

    SPDLOG_DEBUG("Drop {}", id);
}
