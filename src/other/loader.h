#pragma once

#include <thread>
#include <atomic>

#include "opengl/renderer/renderer.h"
#include "other/model.h"
#include "other/texture_data.h"
#include "other/logging.h"

using namespace model;

struct AssetsLoad {
    Rc<Mesh<Vertex>> board_mesh;
    Rc<Mesh<Vertex>> board_paint_mesh;
    Rc<Mesh<VertexP>> node_mesh;
    Rc<Mesh<Vertex>> white_piece_mesh;
    Rc<Mesh<Vertex>> black_piece_mesh;
    Rc<TextureData> board_texture;
    Rc<TextureData> board_paint_texture;
    Rc<TextureData> white_piece_texture;
    Rc<TextureData> black_piece_texture;
    Rc<TextureData> white_indicator_texture;
    Rc<TextureData> black_indicator_texture;
    Rc<TextureData> skybox_px_texture;
    Rc<TextureData> skybox_nx_texture;
    Rc<TextureData> skybox_py_texture;
    Rc<TextureData> skybox_ny_texture;
    Rc<TextureData> skybox_pz_texture;
    Rc<TextureData> skybox_nz_texture;
};

class Loader {
public:
    Loader(std::shared_ptr<AssetsLoad> assets_load)
        : assets_load(assets_load) {}
    ~Loader() = default;

    bool done_loading();
    std::thread& get_thread();
    void start_loading_thread();
private:
    void load();

    std::atomic<bool> loaded = false;
    std::thread loading_thread;

    std::shared_ptr<AssetsLoad> assets_load;
};
