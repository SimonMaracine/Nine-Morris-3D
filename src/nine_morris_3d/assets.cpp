#include "application/platform.h"

namespace assets {
    std::string BOARD_MESH = "data/models/board/board.obj";
    std::string BOARD_PAINT_MESH = "data/models/board/board_paint.obj";
    std::string NODE_MESH = "data/models/node/node.obj";
    std::string WHITE_PIECE_MESH = "data/models/piece/white_piece.obj";
    std::string BLACK_PIECE_MESH = "data/models/piece/black_piece.obj";

    std::string BOARD_WOOD_DIFFUSE_TEXTURE = "data/textures/board/board_wood.png";
    std::string BOARD_PAINT_DIFFUSE_TEXTURE = "data/textures/board/board_paint.png";
    std::string LABELED_BOARD_PAINT_DIFFUSE_TEXTURE = "data/textures/board/board_paint_labeled.png";
    std::string WHITE_PIECE_DIFFUSE_TEXTURE = "data/textures/piece/white_piece.png";
    std::string BLACK_PIECE_DIFFUSE_TEXTURE = "data/textures/piece/black_piece.png";
    std::string BOARD_WOOD_DIFFUSE_TEXTURE_SMALL = "data/textures/board/board_wood-small.png";
    std::string BOARD_PAINT_DIFFUSE_TEXTURE_SMALL = "data/textures/board/board_paint-small.png";
    std::string LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL = "data/textures/board/board_paint_labeled-small.png";
    std::string WHITE_PIECE_DIFFUSE_TEXTURE_SMALL = "data/textures/piece/white_piece-small.png";
    std::string BLACK_PIECE_DIFFUSE_TEXTURE_SMALL = "data/textures/piece/black_piece-small.png";
    std::string WHITE_INDICATOR_TEXTURE = "data/textures/indicator/white_indicator.png";
    std::string BLACK_INDICATOR_TEXTURE = "data/textures/indicator/black_indicator.png";
    std::string SPLASH_SCREEN_TEXTURE = "data/textures/splash_screen/splash_screen.png";
    std::string BOARD_NORMAL_TEXTURE = "data/textures/board/board_normal.png";
    std::string PIECE_NORMAL_TEXTURE = "data/textures/piece/piece_normal.png";

    std::string OPEN_SANS_FONT = "data/fonts/OpenSans/OpenSans-Semibold.ttf";
    std::string GOOD_DOG_PLAIN_FONT = "data/fonts/FH-GoodDogPlain-WTT/GOODDP__.TTF";

    std::string FIELD_PX_TEXTURE = "data/textures/skybox/field/px.png";
    std::string FIELD_NX_TEXTURE = "data/textures/skybox/field/nx.png";
    std::string FIELD_PY_TEXTURE = "data/textures/skybox/field/py.png";
    std::string FIELD_NY_TEXTURE = "data/textures/skybox/field/ny.png";
    std::string FIELD_PZ_TEXTURE = "data/textures/skybox/field/pz.png";
    std::string FIELD_NZ_TEXTURE = "data/textures/skybox/field/nz.png";
    std::string FIELD_PX_TEXTURE_SMALL = "data/textures/skybox/field/px-small.png";
    std::string FIELD_NX_TEXTURE_SMALL = "data/textures/skybox/field/nx-small.png";
    std::string FIELD_PY_TEXTURE_SMALL = "data/textures/skybox/field/py-small.png";
    std::string FIELD_NY_TEXTURE_SMALL = "data/textures/skybox/field/ny-small.png";
    std::string FIELD_PZ_TEXTURE_SMALL = "data/textures/skybox/field/pz-small.png";
    std::string FIELD_NZ_TEXTURE_SMALL = "data/textures/skybox/field/nz-small.png";
    std::string AUTUMN_PX_TEXTURE = "data/textures/skybox/autumn/px.png";
    std::string AUTUMN_NX_TEXTURE = "data/textures/skybox/autumn/nx.png";
    std::string AUTUMN_PY_TEXTURE = "data/textures/skybox/autumn/py.png";
    std::string AUTUMN_NY_TEXTURE = "data/textures/skybox/autumn/ny.png";
    std::string AUTUMN_PZ_TEXTURE = "data/textures/skybox/autumn/pz.png";
    std::string AUTUMN_NZ_TEXTURE = "data/textures/skybox/autumn/nz.png";
    std::string AUTUMN_PX_TEXTURE_SMALL = "data/textures/skybox/autumn/px-small.png";
    std::string AUTUMN_NX_TEXTURE_SMALL = "data/textures/skybox/autumn/nx-small.png";
    std::string AUTUMN_PY_TEXTURE_SMALL = "data/textures/skybox/autumn/py-small.png";
    std::string AUTUMN_NY_TEXTURE_SMALL = "data/textures/skybox/autumn/ny-small.png";
    std::string AUTUMN_PZ_TEXTURE_SMALL = "data/textures/skybox/autumn/pz-small.png";
    std::string AUTUMN_NZ_TEXTURE_SMALL = "data/textures/skybox/autumn/nz-small.png";

    std::string BOARD_VERTEX_SHADER = "data/shaders/board.vert";
    std::string BOARD_FRAGMENT_SHADER = "data/shaders/board.frag";
    std::string BOARD_PAINT_VERTEX_SHADER = "data/shaders/board_paint.vert";
    std::string BOARD_PAINT_FRAGMENT_SHADER = "data/shaders/board_paint.frag";
    std::string PIECE_VERTEX_SHADER = "data/shaders/piece.vert";
    std::string PIECE_FRAGMENT_SHADER = "data/shaders/piece.frag";
    std::string NODE_VERTEX_SHADER = "data/shaders/node.vert";
    std::string NODE_FRAGMENT_SHADER = "data/shaders/node.frag";

    std::string ICON_512 = "data/icons/512x512/ninemorris3d.png";
    std::string ICON_256 = "data/icons/256x256/ninemorris3d.png";
    std::string ICON_128 = "data/icons/128x128/ninemorris3d.png";
    std::string ICON_64 = "data/icons/64x64/ninemorris3d.png";
    std::string ICON_32 = "data/icons/32x32/ninemorris3d.png";

    std::string ARROW_CURSOR = "data/cursors/arrow.png";
    std::string CROSS_CURSOR = "data/cursors/cross.png";

    void maybe_initialize_assets() {
#ifdef PLATFORM_GAME_RELEASE
        static const char* PREFIX = ".dat";

        static constexpr std::array<std::string*, 50> assets = {
            &BOARD_MESH,
            &BOARD_PAINT_MESH,
            &NODE_MESH,
            &WHITE_PIECE_MESH,
            &BLACK_PIECE_MESH,
            &BOARD_WOOD_TEXTURE,
            &BOARD_PAINT_DIFFUSE_TEXTURE,
            &WHITE_PIECE_DIFFUSE_TEXTURE,
            &BLACK_PIECE_DIFFUSE_TEXTURE,
            &BOARD_WOOD_DIFFUSE_TEXTURE_SMALL,
            &BOARD_PAINT_DIFFUSE_TEXTURE_SMALL,
            &WHITE_PIECE_DIFFUSE_TEXTURE_SMALL,
            &BLACK_PIECE_DIFFUSE_TEXTURE_SMALL,
            &WHITE_INDICATOR_TEXTURE,
            &BLACK_INDICATOR_TEXTURE,
            &SPLASH_SCREEN_TEXTURE,
            &FIELD_PX_TEXTURE,
            &FIELD_NX_TEXTURE,
            &FIELD_PY_TEXTURE,
            &FIELD_NY_TEXTURE,
            &FIELD_PZ_TEXTURE,
            &FIELD_NZ_TEXTURE,
            &FIELD_PX_TEXTURE_SMALL,
            &FIELD_NX_TEXTURE_SMALL,
            &FIELD_PY_TEXTURE_SMALL,
            &FIELD_NY_TEXTURE_SMALL,
            &FIELD_PZ_TEXTURE_SMALL,
            &FIELD_NZ_TEXTURE_SMALL,
            &AUTUMN_PX_TEXTURE,
            &AUTUMN_NX_TEXTURE,
            &AUTUMN_PY_TEXTURE,
            &AUTUMN_NY_TEXTURE,
            &AUTUMN_PZ_TEXTURE,
            &AUTUMN_NZ_TEXTURE,
            &AUTUMN_PX_TEXTURE_SMALL,
            &AUTUMN_NX_TEXTURE_SMALL,
            &AUTUMN_PY_TEXTURE_SMALL,
            &AUTUMN_NY_TEXTURE_SMALL,
            &AUTUMN_PZ_TEXTURE_SMALL,
            &AUTUMN_NZ_TEXTURE_SMALL,
            &BOARD_VERTEX_SHADER,
            &BOARD_FRAGMENT_SHADER,
            &BOARD_PAINT_VERTEX_SHADER,
            &BOARD_PAINT_FRAGMENT_SHADER,
            &PIECE_VERTEX_SHADER,
            &PIECE_FRAGMENT_SHADER,
            &NODE_VERTEX_SHADER,
            &NODE_FRAGMENT_SHADER,
            &ARROW_CURSOR,
            &CROSS_CURSOR
        };

        for (std::string* asset : assets) {
            *asset += PREFIX;
        }
#endif
    }
}
