#include <memory>
#include <thread>
#include <atomic>
#include <utility>
#include <cassert>

#include "other/loader.h"
#include "other/model.h"
#include "other/logging.h"

// std::shared_ptr<Assets> Loader::get_assets() {
//     loading_thread.join();
//     std::shared_ptr<Assets> copy = assets;
//     assets = nullptr;
//     return copy;
// }

bool Loader::done_loading() {
    return loaded.load();
}

std::thread& Loader::get_thread() {
    return loading_thread;
}

void Loader::start_loading_thread(std::unordered_map<unsigned int, AssetManager::Asset>& required) {
    this->required = required;
    loading_thread = std::thread(&Loader::load, this);

    SPDLOG_INFO("Loading assets from thread {}...", loading_thread.get_id());
}

void Loader::load() {
    // assets->board_mesh = model::load_model_full("data/models/board/board.obj");
    // assets->board_paint_mesh = model::load_model_full("data/models/board/board_paint.obj");
    // assets->white_piece_mesh = model::load_model_full("data/models/piece/white_piece.obj");
    // assets->black_piece_mesh = model::load_model_full("data/models/piece/black_piece.obj");
    // assets->node_mesh = model::load_model_position("data/models/node/node.obj");

    // SPDLOG_DEBUG("Meshes size: {} bytes",
    //     assets->board_mesh->vertices.capacity() * sizeof(model::FullVertex) +
    //     assets->board_mesh->indices.capacity() * sizeof(unsigned int) +
    //     assets->board_paint_mesh->vertices.capacity() * sizeof(model::FullVertex) +
    //     assets->board_paint_mesh->indices.capacity() * sizeof(unsigned int) +
    //     assets->white_piece_mesh->vertices.capacity() * sizeof(model::FullVertex) +
    //     assets->white_piece_mesh->indices.capacity() * sizeof(unsigned int) +
    //     assets->black_piece_mesh->vertices.capacity() * sizeof(model::FullVertex) +
    //     assets->black_piece_mesh->indices.capacity() * sizeof(unsigned int) +
    //     assets->node_mesh->vertices.capacity() * sizeof(model::PositionVertex) +
    //     assets->node_mesh->indices.capacity() * sizeof(unsigned int)
    // );

    // if (small_textures) {
    //     SPDLOG_DEBUG("Loading small textures");

    //     assets->board_diffuse_data = std::make_shared<TextureData>("data/textures/board/board_wood-small.png", true);
    //     assets->board_paint_data = std::make_shared<TextureData>("data/textures/board/board_paint-small.png", true);

    //     assets->white_piece_diffuse_data = std::make_shared<TextureData>("data/textures/piece/white_piece-small.png", true);
    //     assets->black_piece_diffuse_data = std::make_shared<TextureData>("data/textures/piece/black_piece-small.png", true);
    // } else {
    //     SPDLOG_DEBUG("Loading high resolution textures");

    //     assets->board_diffuse_data = std::make_shared<TextureData>("data/textures/board/board_wood.png", true);
    //     assets->board_paint_data = std::make_shared<TextureData>("data/textures/board/board_paint.png", true);

    //     assets->white_piece_diffuse_data = std::make_shared<TextureData>("data/textures/piece/white_piece.png", true);
    //     assets->black_piece_diffuse_data = std::make_shared<TextureData>("data/textures/piece/black_piece.png", true);
    // }

    // const char* images[6] = {
    //     "data/textures/skybox/right.jpg",
    //     "data/textures/skybox/left.jpg",
    //     "data/textures/skybox/top.jpg",
    //     "data/textures/skybox/bottom.jpg",
    //     "data/textures/skybox/front.jpg",
    //     "data/textures/skybox/back.jpg"
    // };

    // std::array<Rc<TextureData>, 6> skybox_textures;
    // for (int i = 0; i < 6; i++) {
    //     skybox_textures[i] = std::make_shared<TextureData>(images[i], false);
    // }
    // assets->skybox_data = skybox_textures;

    // assets->white_indicator_data = std::make_shared<TextureData>("data/textures/indicator/white_indicator.png", true);
    // assets->black_indicator_data = std::make_shared<TextureData>("data/textures/indicator/black_indicator.png", true);

    using namespace model;

    for (const std::pair<unsigned int, AssetManager::Asset>& pair : required) {
        switch (pair.second.type) {
            case AssetType::FullModel: {
                Rc<Mesh<FullVertex>> mesh = load_model_full(pair.second.file_path);
                asset_manager.all_assets.full_meshes[pair.first] = mesh;

                break;
            }
            case AssetType::PositionModel: {
                Rc<Mesh<PositionVertex>> mesh = load_model_position(pair.second.file_path);
                asset_manager.all_assets.position_meshes[pair.first] = mesh;

                break;
            }
            case AssetType::TextureData: {
                Rc<TextureData> texture_data = std::make_shared<TextureData>(pair.second.file_path, true);
                asset_manager.all_assets.texture_datas[pair.first] = texture_data;

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
