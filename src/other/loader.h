#pragma once

#include <memory>
#include <array>
#include <thread>
#include <atomic>

#include "opengl/renderer/texture.h"
#include "other/model.h"

using namespace model;

struct Assets {
    Mesh board_mesh;
    Mesh board_paint_mesh;
    Mesh white_piece_mesh;
    Mesh black_piece_mesh;
    Mesh node_mesh;

    Rc<TextureData> board_diffuse_data = nullptr;
    Rc<TextureData> white_piece_diffuse_data = nullptr;
    Rc<TextureData> black_piece_diffuse_data = nullptr;
    std::array<Rc<TextureData>, 6> skybox_data;
    Rc<TextureData> board_paint_data = nullptr;

    Rc<TextureData> white_indicator_data = nullptr;
    Rc<TextureData> black_indicator_data = nullptr;
};

class Loader {
public:
    Loader();

    std::shared_ptr<Assets> get_assets();
    bool done_loading();
    std::thread& get_thread();
private:
    void load();

    std::shared_ptr<Assets> assets = std::make_shared<Assets>();
    std::atomic<bool> loaded = false;
    std::thread loading_thread;
};
