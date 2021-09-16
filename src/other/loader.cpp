#include <memory>
#include <thread>
#include <atomic>
#include <utility>
#include <cassert>

#include "other/loader.h"
#include "other/model.h"
#include "other/logging.h"

bool Loader::done_loading() {
    return loaded.load();
}

std::thread& Loader::get_thread() {
    return loading_thread;
}

void Loader::start_loading_thread(const std::unordered_map<unsigned int, AssetManager::Asset>& required) {
    SPDLOG_INFO("Loading assets from separate thread...");

    this->required = required;
    loading_thread = std::thread(&Loader::load, this);
}

void Loader::load() {
    using namespace model;

    for (const std::pair<unsigned int, AssetManager::Asset>& pair : required) {
        switch (pair.second.type) {
            case AssetType::Mesh: {
                Rc<Mesh<Vertex>> mesh = load_model(pair.second.file_path);
                asset_manager.all_assets.meshes[pair.first] = mesh;

                break;
            }
            case AssetType::MeshP: {
                Rc<Mesh<VertexP>> mesh = load_model_position(pair.second.file_path);
                asset_manager.all_assets.meshes_p[pair.first] = mesh;

                break;
            }
            case AssetType::Texture: {
                Rc<TextureData> texture = std::make_shared<TextureData>(pair.second.file_path, false);
                asset_manager.all_assets.textures[pair.first] = texture;

                break;
            }
            case AssetType::TextureFlipped: {
                Rc<TextureData> texture = std::make_shared<TextureData>(pair.second.file_path, true);
                asset_manager.all_assets.textures_flipped[pair.first] = texture;

                break;
            }
            case AssetType::Sound: {
                assert(false);

                break;
            }
        }
    }

    loaded.store(true);
}
