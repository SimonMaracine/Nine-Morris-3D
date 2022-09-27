#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/assets_load_functions.h"
#include "game/assets.h"
#include "game/game_options.h"

using namespace mesh;
using namespace assets;
using namespace paths;
using namespace encrypt;

namespace assets_load_functions {
    void all_start(ConcurrentLoader<game_options::GameOptions>& loader, const game_options::GameOptions& options) {
        loader().mesh_p.load("node_mesh"_h, encr(path_for_assets(NODE_MESH)), true);

        // if (options.normal_mapping) {
        //     loader->get()->board_wood_mesh = load_model_VPTNT(encr(path_for_assets(BOARD_WOOD_MESH)));
        //     loader->get()->board_paint_mesh = load_model_VPTNT(encr(path_for_assets(BOARD_PAINT_MESH)));
        //     loader->get()->white_piece_mesh = load_model_VPTNT(encr(path_for_assets(WHITE_PIECE_MESH)));
        //     loader->get()->black_piece_mesh = load_model_VPTNT(encr(path_for_assets(BLACK_PIECE_MESH)));
        // } else {
        //     loader->get()->board_wood_no_normal_mesh = load_model_VPTN(encr(path_for_assets(BOARD_WOOD_MESH)));
        //     loader->get()->board_paint_no_normal_mesh = load_model_VPTN(encr(path_for_assets(BOARD_PAINT_MESH)));
        //     loader->get()->white_piece_no_normal_mesh = load_model_VPTN(encr(path_for_assets(WHITE_PIECE_MESH)));
        //     loader->get()->black_piece_no_normal_mesh = load_model_VPTN(encr(path_for_assets(BLACK_PIECE_MESH)));
        // }

        loader().texture_data.load("white_indicator_texture"_h, encr(path_for_assets(WHITE_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("black_indicator_texture"_h, encr(path_for_assets(BLACK_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("wait_indicator_texture"_h, encr(path_for_assets(WAIT_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("keyboard_controls_texture"_h, encr(path_for_assets(KEYBOARD_CONTROLS_TEXTURE)), true);
        loader().texture_data.load("keyboard_controls_cross_texture"_h, encr(path_for_assets(KEYBOARD_CONTROLS_CROSS_TEXTURE)), true);

        // if (options.texture_quality == options::NORMAL) {
        //     loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE)), true);
        //     loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE)), true);
        //     loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE)), true);

        //     if (options.normal_mapping) {
        //         loader->get()->board_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
        //         loader->get()->piece_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
        //     }

        //     if (options.labeled_board) {
        //         loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
        //     } else {
        //         loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
        //     }

        //     if (options.skybox == options::FIELD) {
        //         loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PX_TEXTURE)), false);
        //         loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NX_TEXTURE)), false);
        //         loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PY_TEXTURE)), false);
        //         loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NY_TEXTURE)), false);
        //         loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
        //         loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
        //     } else if (options.skybox == options::AUTUMN) {
        //         loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
        //         loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
        //         loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
        //         loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
        //         loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
        //         loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
        //     }
        // } else if (options.texture_quality == options::LOW) {
        //     loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE_SMALL)), true);
        //     loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE_SMALL)), true);
        //     loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE_SMALL)), true);

        //     if (options.normal_mapping) {
        //         loader->get()->board_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
        //         loader->get()->piece_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
        //     }

        //     if (options.labeled_board) {
        //         loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
        //     } else {
        //         loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
        //     }

        //     if (options.skybox == options::FIELD) {
        //         loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
        //     } else if (options.skybox == options::AUTUMN) {
        //         loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
        //         loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
        //     }
        // }

        loader.set_done();
    }

    // void skybox(Loader<AssetsData, options::Options>* loader, const options::Options& options) {
    //     if (options.texture_quality == options::NORMAL) {
    //         if (options.skybox == options::FIELD) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PX_TEXTURE)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NX_TEXTURE)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PY_TEXTURE)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NY_TEXTURE)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
    //         } else if (options.skybox == options::AUTUMN) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
    //         }
    //     } else if (options.texture_quality == options::LOW) {
    //         if (options.skybox == options::FIELD) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
    //         } else if (options.skybox == options::AUTUMN) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
    //         }
    //     }

    //     loader->set_done();
    // }

    // void texture_quality(Loader<AssetsData, options::Options>* loader, const options::Options& options) {
    //     if (options.texture_quality == options::NORMAL) {
    //         loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE)), true);
    //         loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE)), true);
    //         loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE)), true);

    //         if (options.normal_mapping) {
    //             loader->get()->board_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
    //             loader->get()->piece_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
    //         }

    //         if (options.labeled_board) {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
    //         } else {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
    //         }

    //         if (options.skybox == options::FIELD) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PX_TEXTURE)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NX_TEXTURE)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PY_TEXTURE)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NY_TEXTURE)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
    //         } else if (options.skybox == options::AUTUMN) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
    //         }
    //     } else if (options.texture_quality == options::LOW) {
    //         loader->get()->board_wood_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE_SMALL)), true);
    //         loader->get()->white_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE_SMALL)), true);
    //         loader->get()->black_piece_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE_SMALL)), true);

    //         if (options.normal_mapping) {
    //             loader->get()->board_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
    //             loader->get()->piece_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
    //         }

    //         if (options.labeled_board) {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
    //         } else {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
    //         }

    //         if (options.skybox == options::FIELD) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
    //         } else if (options.skybox == options::AUTUMN) {
    //             loader->get()->skybox_px_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nx_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_py_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_ny_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_pz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
    //             loader->get()->skybox_nz_texture = std::make_shared<TextureData>(encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
    //         }
    //     }

    //     loader->set_done();
    // }

    // void board_paint_texture(Loader<AssetsData, options::Options>* loader, const options::Options& options) {
    //     if (options.texture_quality == options::NORMAL) {
    //         if (options.labeled_board) {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
    //         } else {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
    //         }
    //     } else if (options.texture_quality == options::LOW) {
    //         if (options.labeled_board) {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
    //         } else {
    //             loader->get()->board_paint_diff_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
    //         }
    //     }

    //     loader->set_done();
    // }

    // void normal_mapping(Loader<AssetsData, options::Options>* loader, const options::Options& options) {
    //     if (options.normal_mapping) {
    //         loader->get()->board_wood_mesh = load_model_VPTNT(encr(path_for_assets(BOARD_WOOD_MESH)));
    //         loader->get()->board_paint_mesh = load_model_VPTNT(encr(path_for_assets(BOARD_PAINT_MESH)));
    //         loader->get()->white_piece_mesh = load_model_VPTNT(encr(path_for_assets(WHITE_PIECE_MESH)));
    //         loader->get()->black_piece_mesh = load_model_VPTNT(encr(path_for_assets(BLACK_PIECE_MESH)));

    //         if (options.texture_quality == options::NORMAL) {
    //             loader->get()->board_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
    //             loader->get()->piece_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
    //         } else if (options.texture_quality == options::LOW) {
    //             loader->get()->board_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
    //             loader->get()->piece_norm_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
    //         }
            
    //         loader->get()->board_wood_no_normal_mesh = nullptr;
    //         loader->get()->board_paint_no_normal_mesh = nullptr;
    //         loader->get()->white_piece_no_normal_mesh = nullptr;
    //         loader->get()->black_piece_no_normal_mesh = nullptr;
    //     } else {
    //         loader->get()->board_wood_no_normal_mesh = load_model_VPTN(encr(path_for_assets(BOARD_WOOD_MESH)));
    //         loader->get()->board_paint_no_normal_mesh = load_model_VPTN(encr(path_for_assets(BOARD_PAINT_MESH)));
    //         loader->get()->white_piece_no_normal_mesh = load_model_VPTN(encr(path_for_assets(WHITE_PIECE_MESH)));
    //         loader->get()->black_piece_no_normal_mesh = load_model_VPTN(encr(path_for_assets(BLACK_PIECE_MESH)));

    //         loader->get()->board_wood_mesh = nullptr;
    //         loader->get()->board_paint_mesh = nullptr;
    //         loader->get()->white_piece_mesh = nullptr;
    //         loader->get()->black_piece_mesh = nullptr;

    //         loader->get()->board_norm_texture = nullptr;
    //         loader->get()->piece_norm_texture = nullptr;
    //     }

    //     loader->set_done();
    // }
}
