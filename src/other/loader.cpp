#include <memory>
#include <thread>
#include <atomic>
#include <utility>
#include <cassert>

#include "other/loader.h"
#include "other/model.h"
#include "other/texture_data.h"
#include "other/logging.h"
#include "other/assets.h"

bool Loader::done_loading() {
    return loaded.load();
}

std::thread& Loader::get_thread() {
    return loading_thread;
}

void Loader::start_loading_thread() {
    SPDLOG_INFO("Loading assets from separate thread...");

    loading_thread = std::thread(&Loader::load, this);
}

void Loader::load() {
    using namespace model;

    assets_load->board_mesh = load_model(assets::path(assets::BOARD_MESH));
    assets_load->board_paint_mesh = load_model(assets::path(assets::BOARD_PAINT_MESH));
    assets_load->node_mesh = load_model_position(assets::path(assets::NODE_MESH));
    assets_load->white_piece_mesh = load_model(assets::path(assets::WHITE_PIECE_MESH));
    assets_load->black_piece_mesh = load_model(assets::path(assets::BLACK_PIECE_MESH));
    assets_load->board_texture = std::make_shared<TextureData>(assets::path(assets::BOARD_TEXTURE), true);
    assets_load->board_texture_small = std::make_shared<TextureData>(assets::path(assets::BOARD_TEXTURE_SMALL), true);
    assets_load->board_paint_diffuse_texture = std::make_shared<TextureData>(assets::path(assets::BOARD_PAINT_TEXTURE), true);
    assets_load->board_paint_diffuse_texture_small = std::make_shared<TextureData>(assets::path(assets::BOARD_PAINT_TEXTURE_SMALL), true);
    assets_load->white_piece_texture = std::make_shared<TextureData>(assets::path(assets::WHITE_PIECE_TEXTURE), true);
    assets_load->white_piece_texture_small = std::make_shared<TextureData>(assets::path(assets::WHITE_PIECE_TEXTURE_SMALL), true);
    assets_load->black_piece_texture = std::make_shared<TextureData>(assets::path(assets::BLACK_PIECE_TEXTURE), true);
    assets_load->black_piece_texture_small = std::make_shared<TextureData>(assets::path(assets::BLACK_PIECE_TEXTURE_SMALL), true);
    assets_load->white_indicator_texture = std::make_shared<TextureData>(assets::path(assets::WHITE_INDICATOR_TEXTURE), true);
    assets_load->black_indicator_texture = std::make_shared<TextureData>(assets::path(assets::BLACK_INDICATOR_TEXTURE), true);
    assets_load->skybox_px_texture = std::make_shared<TextureData>(assets::path(assets::SKYBOX_PX_TEXTURE), false);
    assets_load->skybox_nx_texture = std::make_shared<TextureData>(assets::path(assets::SKYBOX_NX_TEXTURE), false);
    assets_load->skybox_py_texture = std::make_shared<TextureData>(assets::path(assets::SKYBOX_PY_TEXTURE), false);
    assets_load->skybox_ny_texture = std::make_shared<TextureData>(assets::path(assets::SKYBOX_NY_TEXTURE), false);
    assets_load->skybox_pz_texture = std::make_shared<TextureData>(assets::path(assets::SKYBOX_PZ_TEXTURE), false);
    assets_load->skybox_nz_texture = std::make_shared<TextureData>(assets::path(assets::SKYBOX_NZ_TEXTURE), false);

    loaded.store(true);
}
