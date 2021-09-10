#include <cassert>
#include <memory>
#include <utility>

#include "other/asset_manager.h"
#include "other/logging.h"
#include "other/model.h"
#include "other/texture_data.h"

void AssetManager::define_asset(unsigned int id, AssetType type, const std::string& file_path) {
    assert(asset_declarations.find(id) == asset_declarations.end());

    asset_declarations[id] = { type, file_path };
}

std::string AssetManager::get_file_path(unsigned int id) {
    assert(asset_declarations.find(id) != asset_declarations.end());

    const Asset& asset = asset_declarations[id];
    return asset.file_path;
}

Rc<Mesh<Vertex>> AssetManager::get_mesh(unsigned int id) {
    for (const std::pair<unsigned int, Rc<Mesh<Vertex>>>& pair : all_assets.meshes) {
        if (pair.first == id) {
            return pair.second;
        }
    }

    assert(false);
    return nullptr;
}

Rc<Mesh<VertexP>> AssetManager::get_mesh_p(unsigned int id) {
    for (const std::pair<unsigned int, Rc<Mesh<VertexP>>>& pair : all_assets.meshes_p) {
        if (pair.first == id) {
            return pair.second;
        }
    }

    assert(false);
    return nullptr;
}

Rc<TextureData> AssetManager::get_texture(unsigned int id) {
    for (const std::pair<unsigned int, Rc<TextureData>>& pair : all_assets.textures) {
        if (pair.first == id) {
            return pair.second;
        }
    }

    assert(false);
    return nullptr;
}

Rc<TextureData> AssetManager::get_texture_flipped(unsigned int id) {
    for (const std::pair<unsigned int, Rc<TextureData>>& pair : all_assets.textures_flipped) {
        if (pair.first == id) {
            return pair.second;
        }
    }

    assert(false);
    return nullptr;
}

void AssetManager::load_now(unsigned int id, AssetType type) {
    using namespace model;

    switch (type) {
        case AssetType::Mesh: {
            assert(all_assets.meshes.find(id) == all_assets.meshes.end());

            const Asset& asset = asset_declarations[id];
            assert(asset.type == type);

            Rc<Mesh<Vertex>> mesh = load_model(asset.file_path);
            all_assets.meshes[id] = mesh;

            break;
        }
        case AssetType::MeshP: {
            assert(all_assets.meshes_p.find(id) == all_assets.meshes_p.end());

            const Asset& asset = asset_declarations[id];
            assert(asset.type == type);

            Rc<Mesh<VertexP>> mesh = load_model_position(asset.file_path);
            all_assets.meshes_p[id] = mesh;

            break;
        }
        case AssetType::Texture: {
            assert(all_assets.textures.find(id) == all_assets.textures.end());

            const Asset& asset = asset_declarations[id];
            assert(asset.type == type);

            Rc<TextureData> texture = std::make_shared<TextureData>(asset.file_path, false);
            all_assets.textures[id] = texture;

            break;
        }
        case AssetType::TextureFlipped: {
            assert(all_assets.textures_flipped.find(id) == all_assets.textures_flipped.end());

            const Asset& asset = asset_declarations[id];
            assert(asset.type == type);

            Rc<TextureData> texture = std::make_shared<TextureData>(asset.file_path, true);
            all_assets.textures_flipped[id] = texture;

            break;
        }
        case AssetType::Sound: {
            assert(false);

            break;
        }
    }
}

void AssetManager::drop(unsigned int id, AssetType type) {
    switch (type) {
        case AssetType::Mesh:
            assert(all_assets.meshes.find(id) != all_assets.meshes.end());

            all_assets.meshes.erase(id);

            break;
        case AssetType::MeshP:
            assert(all_assets.meshes_p.find(id) != all_assets.meshes_p.end());

            all_assets.meshes_p.erase(id);

            break;
        case AssetType::Texture:
            assert(all_assets.textures.find(id) != all_assets.textures.end());

            all_assets.textures.erase(id);

            break;
        case AssetType::TextureFlipped:
            assert(all_assets.textures_flipped.find(id) != all_assets.textures_flipped.end());

            all_assets.textures_flipped.erase(id);

            break;
        case AssetType::Sound:
            assert(false);

            break;
    }
}

void AssetManager::require(unsigned int id) {
    assert(asset_declarations.find(id) != asset_declarations.end());

    required[id] = asset_declarations[id];
}

std::unordered_map<unsigned int, AssetManager::Asset> AssetManager::get_required() {
    assert(!required.empty());

    std::unordered_map<unsigned int, Asset> copy = required;
    required.clear();

    return copy;
}
