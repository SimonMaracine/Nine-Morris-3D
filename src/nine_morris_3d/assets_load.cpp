#include "nine_morris_3d/assets_load.h"
#include "other/model.h"
#include "other/texture_data.h"
#include "other/loader.h"
#include "other/assets.h"

namespace assets_load {
    void field(Loader<AssetsLoad>* loader) {
        using namespace model;
        using namespace assets;

        loader->get()->board_mesh = load_model(path(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path(NODE_MESH));
        loader->get()->white_piece_mesh = load_model(path(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path(BLACK_PIECE_MESH));
        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path(BOARD_WOOD_TEXTURE), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE), true);
        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path(BLACK_INDICATOR_TEXTURE), true);
        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path(FIELD_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path(FIELD_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path(FIELD_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path(FIELD_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path(FIELD_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path(FIELD_NZ_TEXTURE), false);

        loader->set_done();
    }

    void field_low_tex(Loader<AssetsLoad>* loader) {
        using namespace model;
        using namespace assets;

        loader->get()->board_mesh = load_model(path(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path(NODE_MESH));
        loader->get()->white_piece_mesh = load_model(path(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path(BLACK_PIECE_MESH));
        loader->get()->board_wood_diff_texture_small = std::make_shared<TextureData>(path(BOARD_WOOD_TEXTURE_SMALL), true);
        loader->get()->board_paint_diff_texture_small = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE_SMALL), true);
        loader->get()->white_piece_diff_texture_small = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE_SMALL), true);
        loader->get()->black_piece_diff_texture_small = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE_SMALL), true);
        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path(BLACK_INDICATOR_TEXTURE), true);
        loader->get()->skybox_px_texture_small = std::make_shared<TextureData>(path(FIELD_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture_small = std::make_shared<TextureData>(path(FIELD_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture_small = std::make_shared<TextureData>(path(FIELD_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture_small = std::make_shared<TextureData>(path(FIELD_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture_small = std::make_shared<TextureData>(path(FIELD_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture_small = std::make_shared<TextureData>(path(FIELD_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }

    void autumn(Loader<AssetsLoad>* loader) {
        using namespace model;
        using namespace assets;

        loader->get()->board_mesh = load_model(path(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path(NODE_MESH));
        loader->get()->white_piece_mesh = load_model(path(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path(BLACK_PIECE_MESH));
        loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(path(BOARD_WOOD_TEXTURE), true);
        loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE), true);
        loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE), true);
        loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE), true);
        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path(BLACK_INDICATOR_TEXTURE), true);
        loader->get()->skybox_px_texture = std::make_shared<TextureData>(path(AUTUMN_PX_TEXTURE), false);
        loader->get()->skybox_nx_texture = std::make_shared<TextureData>(path(AUTUMN_NX_TEXTURE), false);
        loader->get()->skybox_py_texture = std::make_shared<TextureData>(path(AUTUMN_PY_TEXTURE), false);
        loader->get()->skybox_ny_texture = std::make_shared<TextureData>(path(AUTUMN_NY_TEXTURE), false);
        loader->get()->skybox_pz_texture = std::make_shared<TextureData>(path(AUTUMN_PZ_TEXTURE), false);
        loader->get()->skybox_nz_texture = std::make_shared<TextureData>(path(AUTUMN_NZ_TEXTURE), false);

        loader->set_done();
    }

    void autumn_low_tex(Loader<AssetsLoad>* loader) {
        using namespace model;
        using namespace assets;

        loader->get()->board_mesh = load_model(path(BOARD_MESH));
        loader->get()->board_paint_mesh = load_model(path(BOARD_PAINT_MESH));
        loader->get()->node_mesh = load_model_position(path(NODE_MESH));
        loader->get()->white_piece_mesh = load_model(path(WHITE_PIECE_MESH));
        loader->get()->black_piece_mesh = load_model(path(BLACK_PIECE_MESH));
        loader->get()->board_wood_diff_texture_small = std::make_shared<TextureData>(path(BOARD_WOOD_TEXTURE_SMALL), true);
        loader->get()->board_paint_diff_texture_small = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE_SMALL), true);
        loader->get()->white_piece_diff_texture_small = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE_SMALL), true);
        loader->get()->black_piece_diff_texture_small = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE_SMALL), true);
        loader->get()->white_indicator_texture = std::make_shared<TextureData>(path(WHITE_INDICATOR_TEXTURE), true);
        loader->get()->black_indicator_texture = std::make_shared<TextureData>(path(BLACK_INDICATOR_TEXTURE), true);
        loader->get()->skybox_px_texture_small = std::make_shared<TextureData>(path(AUTUMN_PX_TEXTURE_SMALL), false);
        loader->get()->skybox_nx_texture_small = std::make_shared<TextureData>(path(AUTUMN_NX_TEXTURE_SMALL), false);
        loader->get()->skybox_py_texture_small = std::make_shared<TextureData>(path(AUTUMN_PY_TEXTURE_SMALL), false);
        loader->get()->skybox_ny_texture_small = std::make_shared<TextureData>(path(AUTUMN_NY_TEXTURE_SMALL), false);
        loader->get()->skybox_pz_texture_small = std::make_shared<TextureData>(path(AUTUMN_PZ_TEXTURE_SMALL), false);
        loader->get()->skybox_nz_texture_small = std::make_shared<TextureData>(path(AUTUMN_NZ_TEXTURE_SMALL), false);

        loader->set_done();
    }
}
