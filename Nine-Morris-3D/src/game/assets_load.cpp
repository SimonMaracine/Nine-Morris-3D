#include <engine/engine_other.h>

#include "game/assets_load.h"
#include "game/assets.h"
#include "game/game_options.h"
#include "launcher/launcher_options.h"

using namespace mesh;
using namespace assets;
using namespace file_system;
using namespace encrypt;

namespace assets_load {
    void AllStartLoader::load(bool normal_mapping, int texture_quality, bool labeled_board, int skybox) {
        res.mesh.load("node"_H, MeshLoader::P {}, encr(path_for_assets(NODE_MESH)), true);

        if (normal_mapping) {
            res.mesh.load("board_wood"_H, MeshLoader::PTNT {}, encr(path_for_assets(BOARD_WOOD_MESH)));
            res.mesh.load("board_paint"_H, MeshLoader::PTNT {}, encr(path_for_assets(BOARD_PAINT_MESH)));
            res.mesh.load("white_piece"_H, MeshLoader::PTNT {}, encr(path_for_assets(WHITE_PIECE_MESH)));
            res.mesh.load("black_piece"_H, MeshLoader::PTNT {}, encr(path_for_assets(BLACK_PIECE_MESH)));
        } else {
            res.mesh.load("board_wood"_H, MeshLoader::PTN {}, encr(path_for_assets(BOARD_WOOD_MESH)));
            res.mesh.load("board_paint"_H, MeshLoader::PTN {}, encr(path_for_assets(BOARD_PAINT_MESH)));
            res.mesh.load("white_piece"_H, MeshLoader::PTN {}, encr(path_for_assets(WHITE_PIECE_MESH)));
            res.mesh.load("black_piece"_H, MeshLoader::PTN {}, encr(path_for_assets(BLACK_PIECE_MESH)));
        }

        res.texture_data.load("white_indicator"_H, encr(path_for_assets(WHITE_INDICATOR_TEXTURE)), true);
        res.texture_data.load("black_indicator"_H, encr(path_for_assets(BLACK_INDICATOR_TEXTURE)), true);
        res.texture_data.load("wait_indicator"_H, encr(path_for_assets(WAIT_INDICATOR_TEXTURE)), true);
        res.texture_data.load("computer_thinking_indicator"_H, encr(path_for_assets(COMPUTER_THINKING_INDICATOR_TEXTURE)), true);
        res.texture_data.load("keyboard_controls_default"_H, encr(path_for_assets(KEYBOARD_CONTROLS_DEFAULT_TEXTURE)), true);
        res.texture_data.load("keyboard_controls_cross"_H, encr(path_for_assets(KEYBOARD_CONTROLS_CROSS_TEXTURE)), true);

        if (texture_quality == launcher_options::NORMAL) {
            res.texture_data.load("board_wood_diffuse"_H, encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE)), true);
            res.texture_data.load("white_piece_diffuse"_H, encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE)), true);
            res.texture_data.load("black_piece_diffuse"_H, encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE)), true);

            if (normal_mapping) {
                res.texture_data.load("board_normal"_H, encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
                res.texture_data.load("piece_normal"_H, encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
            }

            if (labeled_board) {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            } else {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            }

            if (skybox == game_options::FIELD) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
            } else if (skybox == game_options::AUTUMN) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
            }
        } else if (texture_quality == launcher_options::LOW) {
            res.texture_data.load("board_wood_diffuse"_H, encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE_SMALL)), true);
            res.texture_data.load("white_piece_diffuse"_H, encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE_SMALL)), true);
            res.texture_data.load("black_piece_diffuse"_H, encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE_SMALL)), true);

            if (normal_mapping) {
                res.texture_data.load("board_normal"_H, encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
                res.texture_data.load("piece_normal"_H, encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
            }

            if (labeled_board) {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            } else {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            }

            if (skybox == game_options::FIELD) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
            } else if (skybox == game_options::AUTUMN) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
            }
        }

        res.sound_data.load("piece_place1"_H, encr(path_for_assets(PIECE_PLACE_SOUND1)));
        res.sound_data.load("piece_place2"_H, encr(path_for_assets(PIECE_PLACE_SOUND2)));
        res.sound_data.load("piece_move1"_H, encr(path_for_assets(PIECE_MOVE_SOUND1)));
        res.sound_data.load("piece_move2"_H, encr(path_for_assets(PIECE_MOVE_SOUND2)));
        res.sound_data.load("piece_take"_H, encr(path_for_assets(PIECE_TAKE_SOUND)));
        res.sound_data.load("music"_H, encr(path_for_assets(MUSIC)));

        set_done();
    }

    void SkyboxLoader::load(int texture_quality, int skybox) {
        if (texture_quality == launcher_options::NORMAL) {
            if (skybox == game_options::FIELD) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
            } else if (skybox == game_options::AUTUMN) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
            }
        } else if (texture_quality == launcher_options::LOW) {
            if (skybox == game_options::FIELD) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
            } else if (skybox == game_options::AUTUMN) {
                res.texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
                res.texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
            }
        }

        set_done();
    }

    void BoardPaintTextureLoader::load(int texture_quality, bool labeled_board) {
        if (texture_quality == launcher_options::NORMAL) {
            if (labeled_board) {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            } else {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            }
        } else if (texture_quality == launcher_options::LOW) {
            if (labeled_board) {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            } else {
                res.texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            }
        }

        set_done();
    }
}
