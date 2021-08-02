#pragma once

#include <memory>
#include <utility>
#include <array>
#include <thread>
#include <atomic>

#include "opengl/renderer/texture.h"
#include "other/model.h"

using namespace model;

struct Assets {
    std::tuple<Mesh, Mesh, Mesh, Mesh> meshes;

    std::shared_ptr<TextureData> board_diffuse_texture_data = nullptr;
    std::shared_ptr<TextureData> white_piece_diffuse_data = nullptr;
    std::shared_ptr<TextureData> black_piece_diffuse_data = nullptr;
    std::array<std::shared_ptr<TextureData>, 6> skybox_textures_data;
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
