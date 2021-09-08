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

template<typename T>
Rc<T> AssetManager::get(unsigned int id) {
    for (const std::pair<unsigned int, Rc<Mesh<FullVertex>>>& pair : all_assets.full_meshes) {
        if (pair.first == id) {
            return (Rc<T>) pair.second;
        }
    }

    for (const std::pair<unsigned int, Rc<Mesh<PositionVertex>>>& pair : all_assets.position_meshes) {
        if (pair.first == id) {
            return (Rc<T>) pair.second;
        }
    }

    for (const std::pair<unsigned int, Rc<TextureData>>& pair : all_assets.texture_datas) {
        if (pair.first == id) {
            return (Rc<T>) pair.second;
        }
    }

    assert(false);
    return nullptr;
}

void AssetManager::load_now(unsigned int id, AssetType type) {
    using namespace model;

    switch (type) {
        case AssetType::FullModel: {
            assert(all_assets.full_meshes.find(id) == all_assets.full_meshes.end());

            const Asset& asset = asset_declarations[id];
            assert(asset.type == type);

            Rc<Mesh<FullVertex>> mesh = load_model_full(asset.file_path);
            all_assets.full_meshes[id] = mesh;

            break;
        }
        case AssetType::PositionModel: {
            assert(all_assets.position_meshes.find(id) == all_assets.position_meshes.end());

            const Asset& asset = asset_declarations[id];
            assert(asset.type == type);

            Rc<Mesh<PositionVertex>> mesh = load_model_position(asset.file_path);
            all_assets.position_meshes[id] = mesh;

            break;
        }
        case AssetType::TextureData: {
            assert(all_assets.texture_datas.find(id) == all_assets.texture_datas.end());

            const Asset& asset = asset_declarations[id];
            assert(asset.type == type);

            Rc<TextureData> texture_data = std::make_shared<TextureData>(asset.file_path, true);
            all_assets.texture_datas[id] = texture_data;

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
        case AssetType::FullModel:
            all_assets.full_meshes.erase(id);

            break;
        case AssetType::PositionModel:
            all_assets.position_meshes.erase(id);

            break;
        case AssetType::TextureData:
            all_assets.texture_datas.erase(id);

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
