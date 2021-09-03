#pragma once

#include <memory>
#include <array>
#include <thread>
#include <atomic>

#include "opengl/renderer/texture.h"
#include "other/model.h"
#include "other/logging.h"

using namespace model;

struct Assets {
    ~Assets() { SPDLOG_DEBUG("Deleted assets struct"); }

    Rc<Mesh<FullVertex>> board_mesh;
    Rc<Mesh<FullVertex>> board_paint_mesh;
    Rc<Mesh<FullVertex>> white_piece_mesh;
    Rc<Mesh<FullVertex>> black_piece_mesh;
    Rc<Mesh<PositionVertex>> node_mesh;

    Rc<TextureData> board_diffuse_data = nullptr;
    Rc<TextureData> board_paint_data = nullptr;
    Rc<TextureData> white_piece_diffuse_data = nullptr;
    Rc<TextureData> black_piece_diffuse_data = nullptr;
    std::array<Rc<TextureData>, 6> skybox_data;

    Rc<TextureData> white_indicator_data = nullptr;
    Rc<TextureData> black_indicator_data = nullptr;
};

class Loader {
public:
    Loader() = default;

    std::shared_ptr<Assets> get_assets();
    bool done_loading();
    std::thread& get_thread();
    void start_loading_thread(bool small_textures);
private:
    void load(bool small_textures);

    std::shared_ptr<Assets> assets = std::make_shared<Assets>();
    std::atomic<bool> loaded = false;
    std::thread loading_thread;
};
