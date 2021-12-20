#include "nine_morris_3d/assets_load.h"
#include "other/model.h"
#include "other/texture_data.h"
#include "other/loader.h"
#include "other/assets.h"

void load_assets(Loader<AssetsLoad>* loader) {
    using namespace model;
    using namespace assets;

    loader->assets->board_mesh = load_model(path(BOARD_MESH));
    loader->assets->board_paint_mesh = load_model(path(BOARD_PAINT_MESH));
    loader->assets->node_mesh = load_model_position(path(NODE_MESH));
    loader->assets->white_piece_mesh = load_model(path(WHITE_PIECE_MESH));
    loader->assets->black_piece_mesh = load_model(path(BLACK_PIECE_MESH));
    loader->assets->board_texture = std::make_shared<TextureData>(path(BOARD_TEXTURE), true);
    loader->assets->board_texture_small = std::make_shared<TextureData>(path(BOARD_TEXTURE_SMALL), true);
    loader->assets->board_paint_diffuse_texture = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE), true);
    loader->assets->board_paint_diffuse_texture_small = std::make_shared<TextureData>(path(BOARD_PAINT_TEXTURE_SMALL), true);
    loader->assets->white_piece_texture = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE), true);
    loader->assets->white_piece_texture_small = std::make_shared<TextureData>(path(WHITE_PIECE_TEXTURE_SMALL), true);
    loader->assets->black_piece_texture = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE), true);
    loader->assets->black_piece_texture_small = std::make_shared<TextureData>(path(BLACK_PIECE_TEXTURE_SMALL), true);
    loader->assets->white_indicator_texture = std::make_shared<TextureData>(path(WHITE_INDICATOR_TEXTURE), true);
    loader->assets->black_indicator_texture = std::make_shared<TextureData>(path(BLACK_INDICATOR_TEXTURE), true);
    loader->assets->skybox_px_texture = std::make_shared<TextureData>(path(SKYBOX_PX_TEXTURE), false);
    loader->assets->skybox_nx_texture = std::make_shared<TextureData>(path(SKYBOX_NX_TEXTURE), false);
    loader->assets->skybox_py_texture = std::make_shared<TextureData>(path(SKYBOX_PY_TEXTURE), false);
    loader->assets->skybox_ny_texture = std::make_shared<TextureData>(path(SKYBOX_NY_TEXTURE), false);
    loader->assets->skybox_pz_texture = std::make_shared<TextureData>(path(SKYBOX_PZ_TEXTURE), false);
    loader->assets->skybox_nz_texture = std::make_shared<TextureData>(path(SKYBOX_NZ_TEXTURE), false);

    loader->loaded.store(true);
}
