#pragma once

#include "other/model.h"
#include "other/texture_data.h"
#include "other/loader.h"

using namespace model;

struct AssetsLoad {
    Rc<Mesh<Vertex>> board_mesh;
    Rc<Mesh<Vertex>> board_paint_mesh;
    Rc<Mesh<VertexP>> node_mesh;
    Rc<Mesh<Vertex>> white_piece_mesh;
    Rc<Mesh<Vertex>> black_piece_mesh;
    Rc<TextureData> board_wood_diff_texture;
    Rc<TextureData> board_paint_diff_texture;
    Rc<TextureData> black_piece_diff_texture;
    Rc<TextureData> white_piece_diff_texture;
    Rc<TextureData> board_wood_diff_texture_small;
    Rc<TextureData> board_paint_diff_texture_small;
    Rc<TextureData> white_piece_diff_texture_small;
    Rc<TextureData> black_piece_diff_texture_small;
    Rc<TextureData> white_indicator_texture;
    Rc<TextureData> black_indicator_texture;
    Rc<TextureData> skybox_px_texture;
    Rc<TextureData> skybox_nx_texture;
    Rc<TextureData> skybox_py_texture;
    Rc<TextureData> skybox_ny_texture;
    Rc<TextureData> skybox_pz_texture;
    Rc<TextureData> skybox_nz_texture;
    Rc<TextureData> skybox_px_texture_small;
    Rc<TextureData> skybox_nx_texture_small;
    Rc<TextureData> skybox_py_texture_small;
    Rc<TextureData> skybox_ny_texture_small;
    Rc<TextureData> skybox_pz_texture_small;
    Rc<TextureData> skybox_nz_texture_small;
};

namespace assets_load {
    void field(Loader<AssetsLoad>* loader);
    void field_low_tex(Loader<AssetsLoad>* loader);

    void autumn(Loader<AssetsLoad>* loader);
    void autumn_low_tex(Loader<AssetsLoad>* loader);
}
