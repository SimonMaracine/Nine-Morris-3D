#include "nine_morris_3d/assets_data.h"
#include "nine_morris_3d/assets.h"
#include "nine_morris_3d/paths.h"
#include "other/mesh.h"
#include "other/texture_data.h"
#include "other/loader.h"

using namespace mesh;
using namespace assets;
using namespace paths;

namespace assets_data {
    void field_all(Loader<AssetsData>* loader) {
        loader->get()->board_mesh = load_model(path_for_assets(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path_for_assets(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path_for_assets(NODE_MESH), true);
        loader->get()->white_piece_mesh = load_model(path_for_assets(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path_for_assets(BLACK_PIECE_MESH));

        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE), true);

        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path_for_assets(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path_for_assets(BLACK_INDICATOR_TEXTURE), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NZ_TEXTURE), false);

        loader->set_done();
    }

    void field_low_tex_all(Loader<AssetsData>* loader) {
        loader->get()->board_mesh = load_model(path_for_assets(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path_for_assets(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path_for_assets(NODE_MESH), true);
        loader->get()->white_piece_mesh = load_model(path_for_assets(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path_for_assets(BLACK_PIECE_MESH));

        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE_SMALL), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE_SMALL), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE_SMALL), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE_SMALL), true);

        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path_for_assets(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path_for_assets(BLACK_INDICATOR_TEXTURE), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }

    void autumn_all(Loader<AssetsData>* loader) {
        loader->get()->board_mesh = load_model(path_for_assets(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path_for_assets(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path_for_assets(NODE_MESH), true);
        loader->get()->white_piece_mesh = load_model(path_for_assets(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path_for_assets(BLACK_PIECE_MESH));

        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE), true);

        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path_for_assets(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path_for_assets(BLACK_INDICATOR_TEXTURE), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NZ_TEXTURE), false);

        loader->set_done();
    }

    void autumn_low_tex_all(Loader<AssetsData>* loader) {
        loader->get()->board_mesh = load_model(path_for_assets(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path_for_assets(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path_for_assets(NODE_MESH), true);
        loader->get()->white_piece_mesh = load_model(path_for_assets(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path_for_assets(BLACK_PIECE_MESH));

        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE_SMALL), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE_SMALL), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE_SMALL), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE_SMALL), true);

        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path_for_assets(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path_for_assets(BLACK_INDICATOR_TEXTURE), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }

    void field_skybox(Loader<AssetsData>* loader) {
        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NZ_TEXTURE), false);

        loader->set_done();
    }

    void field_low_tex_skybox(Loader<AssetsData>* loader) {
        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }

    void autumn_skybox(Loader<AssetsData>* loader) {
        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NZ_TEXTURE), false);

        loader->set_done();
    }

    void autumn_low_tex_skybox(Loader<AssetsData>* loader) {
        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }

    void field_texture(Loader<AssetsData>* loader) {
        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NZ_TEXTURE), false);

        loader->set_done();
    }

    void field_low_tex_texture(Loader<AssetsData>* loader) {
        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE_SMALL), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE_SMALL), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE_SMALL), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE_SMALL), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(FIELD_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }

    void autumn_texture(Loader<AssetsData>* loader) {
        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NZ_TEXTURE), false);

        loader->set_done();
    }

    void autumn_low_tex_texture(Loader<AssetsData>* loader) {
        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_WOOD_TEXTURE_SMALL), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path_for_assets(BOARD_PAINT_TEXTURE_SMALL), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(WHITE_PIECE_TEXTURE_SMALL), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path_for_assets(BLACK_PIECE_TEXTURE_SMALL), true);

        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }
}
