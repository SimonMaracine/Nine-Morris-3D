#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "application/platform.h"

namespace assets {
    std::string BOARD_WOOD_MESH = "data/models/board/board.obj.dat";
    std::string BOARD_PAINT_MESH = "data/models/board/board_paint.obj.dat";
    std::string NODE_MESH = "data/models/node/node.obj.dat";
    std::string WHITE_PIECE_MESH = "data/models/piece/white_piece.obj.dat";
    std::string BLACK_PIECE_MESH = "data/models/piece/black_piece.obj.dat";

    std::string BOARD_WOOD_DIFFUSE_TEXTURE = "data/textures/board/wood/board_wood.png.dat";
    std::string BOARD_PAINT_DIFFUSE_TEXTURE = "data/textures/board/paint/board_paint.png.dat";
    std::string LABELED_BOARD_PAINT_DIFFUSE_TEXTURE = "data/textures/board/paint/board_paint_labeled.png.dat";
    std::string BOARD_NORMAL_TEXTURE = "data/textures/board/board_normal.png.dat";
    std::string PIECE_NORMAL_TEXTURE = "data/textures/piece/piece_normal.png.dat";
    std::string WHITE_PIECE_DIFFUSE_TEXTURE = "data/textures/piece/white/white_piece.png.dat";
    std::string BLACK_PIECE_DIFFUSE_TEXTURE = "data/textures/piece/black/black_piece.png.dat";

    std::string BOARD_WOOD_DIFFUSE_TEXTURE_SMALL = "data/textures/board/wood/board_wood-small.png.dat";
    std::string BOARD_PAINT_DIFFUSE_TEXTURE_SMALL = "data/textures/board/paint/board_paint-small.png.dat";
    std::string LABELED_BOARD_PAINT_DIFFUSE_TEXTURE_SMALL = "data/textures/board/paint/board_paint_labeled-small.png.dat";
    std::string BOARD_NORMAL_TEXTURE_SMALL = "data/textures/board/board_normal-small.png.dat";
    std::string PIECE_NORMAL_TEXTURE_SMALL = "data/textures/piece/piece_normal-small.png.dat";
    std::string WHITE_PIECE_DIFFUSE_TEXTURE_SMALL = "data/textures/piece/white/white_piece-small.png.dat";
    std::string BLACK_PIECE_DIFFUSE_TEXTURE_SMALL = "data/textures/piece/black/black_piece-small.png.dat";

    std::string WHITE_INDICATOR_TEXTURE = "data/textures/indicator/white_indicator.png.dat";
    std::string BLACK_INDICATOR_TEXTURE = "data/textures/indicator/black_indicator.png.dat";
    std::string WAIT_INDICATOR_TEXTURE = "data/textures/indicator/wait_indicator.png.dat";
    std::string KEYBOARD_CONTROLS_TEXTURE = "data/textures/keyboard_controls/keyboard_controls.png.dat";
    std::string KEYBOARD_CONTROLS_CROSS_TEXTURE = "data/textures/keyboard_controls/keyboard_controls_cross.png.dat";
    std::string SPLASH_SCREEN_TEXTURE = "data/textures/splash_screen/splash_screen.png.dat";

    std::string OPEN_SANS_FONT = "data/fonts/OpenSans/OpenSans-Semibold.ttf";
    std::string GOOD_DOG_PLAIN_FONT = "data/fonts/FH-GoodDogPlain-WTT/GOODDP__.TTF";

    std::string FIELD_PX_TEXTURE = "data/textures/skybox/field/px.png.dat";
    std::string FIELD_NX_TEXTURE = "data/textures/skybox/field/nx.png.dat";
    std::string FIELD_PY_TEXTURE = "data/textures/skybox/field/py.png.dat";
    std::string FIELD_NY_TEXTURE = "data/textures/skybox/field/ny.png.dat";
    std::string FIELD_PZ_TEXTURE = "data/textures/skybox/field/pz.png.dat";
    std::string FIELD_NZ_TEXTURE = "data/textures/skybox/field/nz.png.dat";
    std::string FIELD_PX_TEXTURE_SMALL = "data/textures/skybox/field/px-small.png.dat";
    std::string FIELD_NX_TEXTURE_SMALL = "data/textures/skybox/field/nx-small.png.dat";
    std::string FIELD_PY_TEXTURE_SMALL = "data/textures/skybox/field/py-small.png.dat";
    std::string FIELD_NY_TEXTURE_SMALL = "data/textures/skybox/field/ny-small.png.dat";
    std::string FIELD_PZ_TEXTURE_SMALL = "data/textures/skybox/field/pz-small.png.dat";
    std::string FIELD_NZ_TEXTURE_SMALL = "data/textures/skybox/field/nz-small.png.dat";
    std::string AUTUMN_PX_TEXTURE = "data/textures/skybox/autumn/px.png.dat";
    std::string AUTUMN_NX_TEXTURE = "data/textures/skybox/autumn/nx.png.dat";
    std::string AUTUMN_PY_TEXTURE = "data/textures/skybox/autumn/py.png.dat";
    std::string AUTUMN_NY_TEXTURE = "data/textures/skybox/autumn/ny.png.dat";
    std::string AUTUMN_PZ_TEXTURE = "data/textures/skybox/autumn/pz.png.dat";
    std::string AUTUMN_NZ_TEXTURE = "data/textures/skybox/autumn/nz.png.dat";
    std::string AUTUMN_PX_TEXTURE_SMALL = "data/textures/skybox/autumn/px-small.png.dat";
    std::string AUTUMN_NX_TEXTURE_SMALL = "data/textures/skybox/autumn/nx-small.png.dat";
    std::string AUTUMN_PY_TEXTURE_SMALL = "data/textures/skybox/autumn/py-small.png.dat";
    std::string AUTUMN_NY_TEXTURE_SMALL = "data/textures/skybox/autumn/ny-small.png.dat";
    std::string AUTUMN_PZ_TEXTURE_SMALL = "data/textures/skybox/autumn/pz-small.png.dat";
    std::string AUTUMN_NZ_TEXTURE_SMALL = "data/textures/skybox/autumn/nz-small.png.dat";

    std::string BOARD_VERTEX_SHADER = "data/shaders/board/board.vert.dat";
    std::string BOARD_FRAGMENT_SHADER = "data/shaders/board/board.frag.dat";
    std::string BOARD_PAINT_VERTEX_SHADER = "data/shaders/board/board_paint.vert.dat";
    std::string BOARD_PAINT_FRAGMENT_SHADER = "data/shaders/board/board_paint.frag.dat";
    std::string PIECE_VERTEX_SHADER = "data/shaders/piece/piece.vert.dat";
    std::string PIECE_FRAGMENT_SHADER = "data/shaders/piece/piece.frag.dat";
    std::string NODE_VERTEX_SHADER = "data/shaders/node/node.vert.dat";
    std::string NODE_FRAGMENT_SHADER = "data/shaders/node/node.frag.dat";
    std::string BOARD_NO_NORMAL_VERTEX_SHADER = "data/shaders/board/board_no_normal.vert.dat";
    std::string BOARD_NO_NORMAL_FRAGMENT_SHADER = "data/shaders/board/board_no_normal.frag.dat";
    std::string BOARD_PAINT_NO_NORMAL_VERTEX_SHADER = "data/shaders/board/board_paint_no_normal.vert.dat";
    std::string BOARD_PAINT_NO_NORMAL_FRAGMENT_SHADER = "data/shaders/board/board_paint_no_normal.frag.dat";
    std::string PIECE_NO_NORMAL_VERTEX_SHADER = "data/shaders/piece/piece_no_normal.vert.dat";
    std::string PIECE_NO_NORMAL_FRAGMENT_SHADER = "data/shaders/piece/piece_no_normal.frag.dat";

    std::string BRIGHT_FILTER_VERTEX_SHADER = "data/shaders/post_processing/bright_filter.vert.dat";
    std::string BRIGHT_FILTER_FRAGMENT_SHADER = "data/shaders/post_processing/bright_filter.frag.dat";
    std::string BLUR_VERTEX_SHADER = "data/shaders/post_processing/blur.vert.dat";
    std::string BLUR_FRAGMENT_SHADER = "data/shaders/post_processing/blur.frag.dat";
    std::string COMBINE_VERTEX_SHADER = "data/shaders/post_processing/combine.vert.dat";
    std::string COMBINE_FRAGMENT_SHADER = "data/shaders/post_processing/combine.frag.dat";

    std::string ICON_512 = "data/icons/512x512/ninemorris3d.png";
    std::string ICON_256 = "data/icons/256x256/ninemorris3d.png";
    std::string ICON_128 = "data/icons/128x128/ninemorris3d.png";
    std::string ICON_64 = "data/icons/64x64/ninemorris3d.png";
    std::string ICON_32 = "data/icons/32x32/ninemorris3d.png";

    std::string ARROW_CURSOR = "data/cursors/arrow/arrow.png.dat";
    std::string CROSS_CURSOR = "data/cursors/cross/cross.png.dat";
}
