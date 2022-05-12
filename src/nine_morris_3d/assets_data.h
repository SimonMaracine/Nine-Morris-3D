#pragma once

#include <memory>

#include "other/mesh.h"
#include "other/texture_data.h"
#include "other/loader.h"

using namespace mesh;

struct AssetsData {
    std::shared_ptr<Mesh<Vertex>> board_mesh;
    std::shared_ptr<Mesh<Vertex>> board_paint_mesh;
    std::shared_ptr<Mesh<VertexP>> node_mesh;
    std::shared_ptr<Mesh<Vertex>> white_piece_mesh;
    std::shared_ptr<Mesh<Vertex>> black_piece_mesh;
    std::shared_ptr<TextureData> board_wood_diff_texture;
    std::shared_ptr<TextureData> board_paint_diff_texture;
    std::shared_ptr<TextureData> black_piece_diff_texture;
    std::shared_ptr<TextureData> white_piece_diff_texture;
    std::shared_ptr<TextureData> white_indicator_texture;
    std::shared_ptr<TextureData> black_indicator_texture;
    std::shared_ptr<TextureData> skybox_px_texture;
    std::shared_ptr<TextureData> skybox_nx_texture;
    std::shared_ptr<TextureData> skybox_py_texture;
    std::shared_ptr<TextureData> skybox_ny_texture;
    std::shared_ptr<TextureData> skybox_pz_texture;
    std::shared_ptr<TextureData> skybox_nz_texture;
};

namespace assets_data {
    void field_all(Loader<AssetsData>* loader);
    void field_low_tex_all(Loader<AssetsData>* loader);

    void autumn_all(Loader<AssetsData>* loader);
    void autumn_low_tex_all(Loader<AssetsData>* loader);

    void field_skybox(Loader<AssetsData>* loader);
    void field_low_tex_skybox(Loader<AssetsData>* loader);

    void autumn_skybox(Loader<AssetsData>* loader);
    void autumn_low_tex_skybox(Loader<AssetsData>* loader);
}
