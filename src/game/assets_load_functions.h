#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/game_options.h"

using namespace mesh;

// struct AssetsData {
    // std::shared_ptr<Mesh<P>> node_mesh;
    // std::shared_ptr<Mesh<PTNT>> board_wood_mesh;
    // std::shared_ptr<Mesh<PTNT>> board_paint_mesh;
    // std::shared_ptr<Mesh<PTNT>> white_piece_mesh;
    // std::shared_ptr<Mesh<PTNT>> black_piece_mesh;

    // std::shared_ptr<Mesh<PTN>> board_wood_no_normal_mesh;
    // std::shared_ptr<Mesh<PTN>> board_paint_no_normal_mesh;
    // std::shared_ptr<Mesh<PTN>> white_piece_no_normal_mesh;
    // std::shared_ptr<Mesh<PTN>> black_piece_no_normal_mesh;

    // std::shared_ptr<TextureData> board_wood_diff_texture;
    // std::shared_ptr<TextureData> board_paint_diff_texture;
    // std::shared_ptr<TextureData> black_piece_diff_texture;
    // std::shared_ptr<TextureData> white_piece_diff_texture;
    // std::shared_ptr<TextureData> board_norm_texture;
    // std::shared_ptr<TextureData> piece_norm_texture;

    // std::shared_ptr<TextureData> white_indicator_texture;
    // std::shared_ptr<TextureData> black_indicator_texture;
    // std::shared_ptr<TextureData> wait_indicator_texture;
    // std::shared_ptr<TextureData> keyboard_controls_texture;
    // std::shared_ptr<TextureData> keyboard_controls_cross_texture;

    // std::shared_ptr<TextureData> skybox_px_texture;
    // std::shared_ptr<TextureData> skybox_nx_texture;
    // std::shared_ptr<TextureData> skybox_py_texture;
    // std::shared_ptr<TextureData> skybox_ny_texture;
    // std::shared_ptr<TextureData> skybox_pz_texture;
    // std::shared_ptr<TextureData> skybox_nz_texture;
// };

namespace assets_load_functions {
    void all_start(ConcurrentLoader<game_options::GameOptions>& loader, const game_options::GameOptions& options);
    // void skybox(Loader<AssetsData, options::Options>* loader, const options::Options& options);
    // void texture_quality(Loader<AssetsData, options::Options>* loader, const options::Options& options);
    // void board_paint_texture(Loader<AssetsData, options::Options>* loader, const options::Options& options);
    // void normal_mapping(Loader<AssetsData, options::Options>* loader, const options::Options& options);
}
