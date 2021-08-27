#include <memory>
#include <thread>
#include <atomic>

#include "other/loader.h"
#include "other/model.h"
#include "other/logging.h"

Loader::Loader() {
    SPDLOG_INFO("Loading assets...");

    loading_thread = std::thread(&Loader::load, this);
}

std::shared_ptr<Assets> Loader::get_assets() {
    loading_thread.join();
    return assets;
}

bool Loader::done_loading() {
    return loaded.load(std::memory_order_relaxed);
}

std::thread& Loader::get_thread() {
    return loading_thread;
}

void Loader::load() {
    assets->meshes = model::load_model("data/models/board.obj");

    assets->board_diffuse_texture_data =
        std::make_shared<TextureData>("data/textures/board.png", true);

    assets->white_piece_diffuse_data =
        std::make_shared<TextureData>("data/textures/white_piece.png", true);

    assets->black_piece_diffuse_data =
        std::make_shared<TextureData>("data/textures/black_piece.png", true);

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
    assets->skybox_textures_data = skybox_textures;

    loaded.store(true, std::memory_order_relaxed);
}
