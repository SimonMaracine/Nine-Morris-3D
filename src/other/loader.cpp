#include <memory>
#include <thread>
#include <atomic>

#include "other/loader.h"
#include "other/model.h"
#include "other/logging.h"

std::shared_ptr<Assets> Loader::get_assets() {
    loading_thread.join();
    std::shared_ptr<Assets> copy = assets;
    assets = nullptr;
    return copy;
}

bool Loader::done_loading() {
    return loaded.load(std::memory_order_relaxed);
}

std::thread& Loader::get_thread() {
    return loading_thread;
}

void Loader::start_loading_thread() {
    SPDLOG_INFO("Loading assets...");

    loading_thread = std::thread(&Loader::load, this);
}

void Loader::load() {
    assets->board_mesh = model::load_model_full("data/models/board.obj");
    assets->board_paint_mesh = model::load_model_full("data/models/board_paint.obj");
    assets->white_piece_mesh = model::load_model_full("data/models/white_piece.obj");
    assets->black_piece_mesh = model::load_model_full("data/models/black_piece.obj");
    assets->node_mesh = model::load_model_position("data/models/node.obj");

    SPDLOG_DEBUG("Meshes size: {} bytes",
        assets->board_mesh->vertices.capacity() * sizeof(model::FullVertex) +
        assets->board_mesh->indices.capacity() * sizeof(unsigned int) +
        assets->board_paint_mesh->vertices.capacity() * sizeof(model::FullVertex) +
        assets->board_paint_mesh->indices.capacity() * sizeof(unsigned int) +
        assets->white_piece_mesh->vertices.capacity() * sizeof(model::FullVertex) +
        assets->white_piece_mesh->indices.capacity() * sizeof(unsigned int) +
        assets->black_piece_mesh->vertices.capacity() * sizeof(model::FullVertex) +
        assets->black_piece_mesh->indices.capacity() * sizeof(unsigned int) +
        assets->node_mesh->vertices.capacity() * sizeof(model::PositionVertex) +
        assets->node_mesh->indices.capacity() * sizeof(unsigned int)
    );

    assets->board_diffuse_data = std::make_shared<TextureData>("data/textures/board_wood.png", true);

    assets->white_piece_diffuse_data = std::make_shared<TextureData>("data/textures/white_piece.png", true);
    assets->black_piece_diffuse_data = std::make_shared<TextureData>("data/textures/black_piece.png", true);

    const char* images[6] = {
        "data/textures/skybox/right.jpg",
        "data/textures/skybox/left.jpg",
        "data/textures/skybox/top.jpg",
        "data/textures/skybox/bottom.jpg",
        "data/textures/skybox/front.jpg",
        "data/textures/skybox/back.jpg"
    };

    std::array<Rc<TextureData>, 6> skybox_textures;
    for (int i = 0; i < 6; i++) {
        skybox_textures[i] = std::make_shared<TextureData>(images[i], false);
    }
    assets->skybox_data = skybox_textures;

    assets->board_paint_data = std::make_shared<TextureData>("data/textures/board_paint.png", true);

    assets->white_indicator_data = std::make_shared<TextureData>("data/textures/white_indicator.png", true);
    assets->black_indicator_data = std::make_shared<TextureData>("data/textures/black_indicator.png", true);

    loaded.store(true, std::memory_order_relaxed);
}
