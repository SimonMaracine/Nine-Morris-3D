#pragma once

#include <thread>
#include <atomic>
#include <functional>

#include "other/logging.h"

// #include "other/model.h"
// #include "other/texture_data.h"

// using namespace model;

// struct AssetsLoad {
//     Rc<Mesh<Vertex>> board_mesh;
//     Rc<Mesh<Vertex>> board_paint_mesh;
//     Rc<Mesh<VertexP>> node_mesh;
//     Rc<Mesh<Vertex>> white_piece_mesh;
//     Rc<Mesh<Vertex>> black_piece_mesh;
//     Rc<TextureData> board_texture;
//     Rc<TextureData> board_texture_small;
//     Rc<TextureData> board_paint_diffuse_texture;
//     Rc<TextureData> board_paint_diffuse_texture_small;
//     Rc<TextureData> white_piece_texture;
//     Rc<TextureData> white_piece_texture_small;
//     Rc<TextureData> black_piece_texture;
//     Rc<TextureData> black_piece_texture_small;
//     Rc<TextureData> white_indicator_texture;
//     Rc<TextureData> black_indicator_texture;
//     Rc<TextureData> skybox_px_texture;
//     Rc<TextureData> skybox_nx_texture;
//     Rc<TextureData> skybox_py_texture;
//     Rc<TextureData> skybox_ny_texture;
//     Rc<TextureData> skybox_pz_texture;
//     Rc<TextureData> skybox_nz_texture;
// };

template<typename Assets>
class Loader {
public:
    Loader(std::shared_ptr<Assets> assets, std::function<void(Loader<Assets>*)> load_function)
        : assets(assets), load_function(load_function) {}
    ~Loader() = default;

    bool done_loading() const {
        return loaded.load();
    }

    std::thread& get_thread() {
        return loading_thread;
    }

    void start_loading_thread() {
        SPDLOG_INFO("Loading assets from separate thread...");

        loading_thread = std::thread(load_function, this);
    }

    std::atomic<bool> loaded = false;
    std::shared_ptr<Assets> assets;
private:
    std::function<void(Loader<Assets>*)> load_function;
    std::thread loading_thread;
};
