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
    void all_start(AllStartLoader& loader, bool normal_mapping, int texture_quality, bool labeled_board, int skybox) {
        loader().mesh.load("node"_H, MeshLoader::P {}, encr(path_for_assets(NODE_MESH)), true);

        if (normal_mapping) {
            loader().mesh.load("board_wood"_H, MeshLoader::PTNT {}, encr(path_for_assets(BOARD_WOOD_MESH)));
            loader().mesh.load("board_paint"_H, MeshLoader::PTNT {}, encr(path_for_assets(BOARD_PAINT_MESH)));
            loader().mesh.load("white_piece"_H, MeshLoader::PTNT {}, encr(path_for_assets(WHITE_PIECE_MESH)));
            loader().mesh.load("black_piece"_H, MeshLoader::PTNT {}, encr(path_for_assets(BLACK_PIECE_MESH)));
        } else {
            loader().mesh.load("board_wood"_H, MeshLoader::PTN {}, encr(path_for_assets(BOARD_WOOD_MESH)));
            loader().mesh.load("board_paint"_H, MeshLoader::PTN {}, encr(path_for_assets(BOARD_PAINT_MESH)));
            loader().mesh.load("white_piece"_H, MeshLoader::PTN {}, encr(path_for_assets(WHITE_PIECE_MESH)));
            loader().mesh.load("black_piece"_H, MeshLoader::PTN {}, encr(path_for_assets(BLACK_PIECE_MESH)));
        }

        loader().texture_data.load("white_indicator"_H, encr(path_for_assets(WHITE_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("black_indicator"_H, encr(path_for_assets(BLACK_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("wait_indicator"_H, encr(path_for_assets(WAIT_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("computer_thinking_indicator"_H, encr(path_for_assets(COMPUTER_THINKING_INDICATOR_TEXTURE)), true);
        loader().texture_data.load("keyboard_controls_default"_H, encr(path_for_assets(KEYBOARD_CONTROLS_DEFAULT_TEXTURE)), true);
        loader().texture_data.load("keyboard_controls_cross"_H, encr(path_for_assets(KEYBOARD_CONTROLS_CROSS_TEXTURE)), true);

        if (texture_quality == launcher_options::NORMAL) {
            loader().texture_data.load("board_wood_diffuse"_H, encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE)), true);
            loader().texture_data.load("white_piece_diffuse"_H, encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE)), true);
            loader().texture_data.load("black_piece_diffuse"_H, encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE)), true);

            if (normal_mapping) {
                loader().texture_data.load("board_normal"_H, encr(path_for_assets(BOARD_NORMAL_TEXTURE)), true);
                loader().texture_data.load("piece_normal"_H, encr(path_for_assets(PIECE_NORMAL_TEXTURE)), true);
            }

            if (labeled_board) {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            } else {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            }

            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
            }
        } else if (texture_quality == launcher_options::LOW) {
            loader().texture_data.load("board_wood_diffuse"_H, encr(path_for_assets(BOARD_WOOD_DIFFUSE_TEXTURE_SMALL)), true);
            loader().texture_data.load("white_piece_diffuse"_H, encr(path_for_assets(WHITE_PIECE_DIFFUSE_TEXTURE_SMALL)), true);
            loader().texture_data.load("black_piece_diffuse"_H, encr(path_for_assets(BLACK_PIECE_DIFFUSE_TEXTURE_SMALL)), true);

            if (normal_mapping) {
                loader().texture_data.load("board_normal"_H, encr(path_for_assets(BOARD_NORMAL_TEXTURE_SMALL)), true);
                loader().texture_data.load("piece_normal"_H, encr(path_for_assets(PIECE_NORMAL_TEXTURE_SMALL)), true);
            }

            if (labeled_board) {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            } else {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            }

            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
            }
        }

        loader().sound_data.load("piece_place1"_H, encr(path_for_assets(PIECE_PLACE_SOUND1)));
        loader().sound_data.load("piece_place2"_H, encr(path_for_assets(PIECE_PLACE_SOUND2)));
        loader().sound_data.load("piece_move1"_H, encr(path_for_assets(PIECE_MOVE_SOUND1)));
        loader().sound_data.load("piece_move2"_H, encr(path_for_assets(PIECE_MOVE_SOUND2)));
        loader().sound_data.load("piece_take"_H, encr(path_for_assets(PIECE_TAKE_SOUND)));
        loader().sound_data.load("music"_H, encr(path_for_assets(MUSIC)));

        loader.set_done();
    }

    void skybox(SkyboxLoader& loader, int texture_quality, int skybox) {
        if (texture_quality == launcher_options::NORMAL) {
            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE)), false);
            }
        } else if (texture_quality == launcher_options::LOW) {
            if (skybox == game_options::FIELD) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(FIELD_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(FIELD_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(FIELD_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(FIELD_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(FIELD_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(FIELD_NZ_TEXTURE_SMALL)), false);
            } else if (skybox == game_options::AUTUMN) {
                loader().texture_data.load("skybox_px"_H, encr(path_for_assets(AUTUMN_PX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nx"_H, encr(path_for_assets(AUTUMN_NX_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_py"_H, encr(path_for_assets(AUTUMN_PY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_ny"_H, encr(path_for_assets(AUTUMN_NY_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_pz"_H, encr(path_for_assets(AUTUMN_PZ_TEXTURE_SMALL)), false);
                loader().texture_data.load("skybox_nz"_H, encr(path_for_assets(AUTUMN_NZ_TEXTURE_SMALL)), false);
            }
        }

        loader.set_done();
    }

    void board_paint_texture(BoardPaintTextureLoader& loader, int texture_quality, bool labeled_board) {
        if (texture_quality == launcher_options::NORMAL) {
            if (labeled_board) {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            } else {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE)), true);
            }
        } else if (texture_quality == launcher_options::LOW) {
            if (labeled_board) {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            } else {
                loader().texture_data.load("board_paint_diffuse"_H, encr(path_for_assets(BOARD_PAINT_DIFFUSE_TEXTURE_SMALL)), true);
            }
        }

        loader.set_done();
    }
}
