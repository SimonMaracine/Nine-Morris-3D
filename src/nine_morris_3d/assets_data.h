#pragma once

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
    void all_field(Loader<AssetsData>* loader);
    void all_field_small(Loader<AssetsData>* loader);

    void all_autumn(Loader<AssetsData>* loader);
    void all_autumn_small(Loader<AssetsData>* loader);

    // ---

    void skybox_field(Loader<AssetsData>* loader);
    void skybox_field_small(Loader<AssetsData>* loader);

    void skybox_autumn(Loader<AssetsData>* loader);
    void skybox_autumn_small(Loader<AssetsData>* loader);

    // ---

    void texture_quality_field(Loader<AssetsData>* loader);
    void texture_quality_field_small(Loader<AssetsData>* loader);

    void texture_quality_autumn(Loader<AssetsData>* loader);
    void texture_quality_autumn_small(Loader<AssetsData>* loader);

    // ---

    void non_labeled_board_texture(Loader<AssetsData>* loader);
    void non_labeled_board_texture_small(Loader<AssetsData>* loader);

    void labeled_board_texture(Loader<AssetsData>* loader);
    void labeled_board_texture_small(Loader<AssetsData>* loader);
}
