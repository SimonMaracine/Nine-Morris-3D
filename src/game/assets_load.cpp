#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/assets_load.h"
#include "game/assets.h"
#include "game/game_options.h"
#include "launcher/launcher_options.h"

using namespace mesh;
using namespace assets;
using namespace paths;
using namespace encrypt;

namespace assets_load {
    void all_start(AllStartLoader& loader, bool normal_mapping, int texture_quality, bool labeled_board, int skybox) {
        loader().mesh_p.load("node_mesh"_h, encr(path_for_assets(NODE_MESH)), true);

        if (normal_mapping) {
            loader().mesh_ptnt.load("board_wood_mesh"_h, encr(path_for_assets(BOARD_WOOD_MESH)));
            loader().mesh_ptnt.load("board_paint_mesh"_h, encr(path_for_assets(BOARD_PAINT_MESH)));
            loader().mesh_ptnt.load("white_piece_mesh"_h, encr(path_for_assets(WHITE_PIECE_MESH)));
            loader().mesh_ptnt.load("black_piece_mesh"_h, encr(path_for_assets(BLACK_PIECE_MESH)));
        } else {
            loader().mesh_ptn.load("board_wood_mesh"_h, encr(path_for_assets(BOARD_WOOD_MESH)));
            loader().mesh_ptn.load("board_paint_mesh"_h, encr(path_for_assets(BOARD_PAINT_MESH)));
            loader().mesh_ptn.load("white_piece_mesh"_h, encr(path_for_assets(WHITE_PIECE_MESH)));
            loader().mesh_ptn.load("black_piece_mesh"_h, encr(path_for_assets(BLACK_PIECE_MESH)));
        }

        loader().texture_data.load("white_indicator_texture"_h, encr(path_for_assets(WHITE_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("black_indicator_texture"_h, encr(path_for_assets(BLACK_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("wait_indicator_texture"_h, encr(path_for_assets(WAIT_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("computer_thinking_indicator_texture"_h, encr(path_for_assets(COMPUTER_THINKING_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("keyboard_controls_texture"_h, encr(path_for_assets(KEYBOARD_CONTROLS_TEXTURE)), true);
        loader().texture_data.load("keyboard_controls_cross_texture"_h, encr(path_for_assets(KEYBOARD_CONTROLS_CROSS_TEXTURE)), true);

        if (texture_quality == launcher_options::NORMAL) {
            loader().texture_data.load("board_wood_diffuse_texture"_h, encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE)), true);
            loader().texture_data.load("white_piece_diffuse_texture"_h, encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE)), true);
            loader().texture_data.load("black_piece_diffuse_texture"_h, encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE)), true);

            if (normal_mapping) {
                loader().texture_data.load("board_normal_texture"_h, encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
                loader().texture_data.load("piece_normal_texture"_h, encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
            }

            if (labeled_board) {
                loader().texture_data.load("board_paint_diffuse_texture"_h, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            } else {
                loader().texture_data.load("board_paint_diffuse_texture"_h, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            }

            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(FIELD_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(FIELD_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(FIELD_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(FIELD_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
            }
        } else if (texture_quality == launcher_options::LOW) {
            loader().texture_data.load("board_wood_diffuse_texture"_h, encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE_SMALL)), true);
            loader().texture_data.load("white_piece_diffuse_texture"_h, encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE_SMALL)), true);
            loader().texture_data.load("black_piece_diffuse_texture"_h, encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE_SMALL)), true);

            if (normal_mapping) {
                loader().texture_data.load("board_normal_texture"_h, encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
                loader().texture_data.load("piece_normal_texture"_h, encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
            }

            if (labeled_board) {
                loader().texture_data.load("board_paint_diffuse_texture"_h, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            } else {
                loader().texture_data.load("board_paint_diffuse_texture"_h, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            }

            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
            }
        }

        loader.set_done();
    }

    void skybox(SkyboxLoader& loader, int texture_quality, int skybox) {
        if (texture_quality == launcher_options::NORMAL) {
            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(FIELD_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(FIELD_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(FIELD_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(FIELD_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
            }
        } else if (texture_quality == launcher_options::LOW) {
            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px_texture"_h, encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx_texture"_h, encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py_texture"_h, encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny_texture"_h, encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz_texture"_h, encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz_texture"_h, encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
            }
        }

        loader.set_done();
    }

    // void texture_quality(Loader<AssetsData, options::Options>* loader, const options::Options& options) {
    //     if (options.texture_quality == options::NORMAL) {
    //         loader->get()->board_wood_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE)), true);
    //         loader->get()->white_piece_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE)), true);
    //         loader->get()->black_piece_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE)), true);

    //         if (options.normal_mapping) {
    //             loader->get()->board_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
    //             loader->get()->piece_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
    //         }

    //         if (options.labeled_board) {
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
    //         } else {
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
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
    //         loader->get()->board_wood_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE_SMALL)), true);
    //         loader->get()->white_piece_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE_SMALL)), true);
    //         loader->get()->black_piece_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE_SMALL)), true);

    //         if (options.normal_mapping) {
    //             loader->get()->board_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
    //             loader->get()->piece_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
    //         }

    //         if (options.labeled_board) {
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
    //         } else {
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
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
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
    //         } else {
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
    //         }
    //     } else if (options.texture_quality == options::LOW) {
    //         if (options.labeled_board) {
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
    //         } else {
    //             loader->get()->board_paint_diffuse_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
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
    //             loader->get()->board_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
    //             loader->get()->piece_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
    //         } else if (options.texture_quality == options::LOW) {
    //             loader->get()->board_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
    //             loader->get()->piece_normal_texture = std::make_shared<TextureData>(encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
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

    //         loader->get()->board_normal_texture = nullptr;
    //         loader->get()->piece_normal_texture = nullptr;
    //     }

    //     loader->set_done();
    // }
}
