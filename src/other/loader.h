#pragma once

#include <thread>
#include <atomic>
#include <unordered_map>

#include "other/asset_manager.h"
#include "other/logging.h"

using namespace model;

// struct Assets {
//     ~Assets() { SPDLOG_DEBUG("Deleted assets struct"); }

//     Rc<Mesh<FullVertex>> board_mesh;
//     Rc<Mesh<FullVertex>> board_paint_mesh;
//     Rc<Mesh<FullVertex>> white_piece_mesh;
//     Rc<Mesh<FullVertex>> black_piece_mesh;
//     Rc<Mesh<PositionVertex>> node_mesh;

//     Rc<TextureData> board_diffuse_data = nullptr;
//     Rc<TextureData> board_paint_data = nullptr;
//     Rc<TextureData> white_piece_diffuse_data = nullptr;
//     Rc<TextureData> black_piece_diffuse_data = nullptr;
//     std::array<Rc<TextureData>, 6> skybox_data;

//     Rc<TextureData> white_indicator_data = nullptr;
//     Rc<TextureData> black_indicator_data = nullptr;
// };

class Loader {
public:
    Loader(AssetManager& asset_manager)
        : asset_manager(asset_manager) {}
    ~Loader() = default;

    // std::shared_ptr<Assets> get_assets();
    bool done_loading();
    std::thread& get_thread();
    void start_loading_thread(std::unordered_map<unsigned int, AssetManager::Asset>& required);
private:
    void load();

    std::unordered_map<unsigned int, AssetManager::Asset> required;
    std::atomic<bool> loaded = false;
    std::thread loading_thread;

    AssetManager& asset_manager;
};
