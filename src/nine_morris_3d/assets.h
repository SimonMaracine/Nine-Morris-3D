#pragma once

#include <string>

#include "application/platform.h"

namespace assets {
    std::string path(const char* file_path);

    extern const char* BOARD_MESH;
    extern const char* BOARD_PAINT_MESH;
    extern const char* NODE_MESH;
    extern const char* WHITE_PIECE_MESH;
    extern const char* BLACK_PIECE_MESH;
    extern const char* BOARD_WOOD_TEXTURE;
    extern const char* BOARD_PAINT_TEXTURE;
    extern const char* WHITE_PIECE_TEXTURE;
    extern const char* BLACK_PIECE_TEXTURE;
    extern const char* BOARD_WOOD_TEXTURE_SMALL;
    extern const char* BOARD_PAINT_TEXTURE_SMALL;
    extern const char* WHITE_PIECE_TEXTURE_SMALL;
    extern const char* BLACK_PIECE_TEXTURE_SMALL;
    extern const char* WHITE_INDICATOR_TEXTURE;
    extern const char* BLACK_INDICATOR_TEXTURE;
    extern const char* SPLASH_SCREEN_TEXTURE;
    extern const char* OPEN_SANS_FONT;
    extern const char* GOOD_DOG_PLAIN_FONT;

    extern const char* FIELD_PX_TEXTURE;
    extern const char* FIELD_NX_TEXTURE;
    extern const char* FIELD_PY_TEXTURE;
    extern const char* FIELD_NY_TEXTURE;
    extern const char* FIELD_PZ_TEXTURE;
    extern const char* FIELD_NZ_TEXTURE;
    extern const char* FIELD_PX_TEXTURE_SMALL;
    extern const char* FIELD_NX_TEXTURE_SMALL;
    extern const char* FIELD_PY_TEXTURE_SMALL;
    extern const char* FIELD_NY_TEXTURE_SMALL;
    extern const char* FIELD_PZ_TEXTURE_SMALL;
    extern const char* FIELD_NZ_TEXTURE_SMALL;
    extern const char* AUTUMN_PX_TEXTURE;
    extern const char* AUTUMN_NX_TEXTURE;
    extern const char* AUTUMN_PY_TEXTURE;
    extern const char* AUTUMN_NY_TEXTURE;
    extern const char* AUTUMN_PZ_TEXTURE;
    extern const char* AUTUMN_NZ_TEXTURE;
    extern const char* AUTUMN_PX_TEXTURE_SMALL;
    extern const char* AUTUMN_NX_TEXTURE_SMALL;
    extern const char* AUTUMN_PY_TEXTURE_SMALL;
    extern const char* AUTUMN_NY_TEXTURE_SMALL;
    extern const char* AUTUMN_PZ_TEXTURE_SMALL;
    extern const char* AUTUMN_NZ_TEXTURE_SMALL;

    extern const char* BOARD_VERTEX_SHADER;
    extern const char* BOARD_FRAGMENT_SHADER;
    extern const char* BOARD_PAINT_VERTEX_SHADER;
    extern const char* BOARD_PAINT_FRAGMENT_SHADER;
    extern const char* PIECE_VERTEX_SHADER;
    extern const char* PIECE_FRAGMENT_SHADER;
    extern const char* SHADOW_VERTEX_SHADER;
    extern const char* SHADOW_FRAGMENT_SHADER;
    extern const char* SCREEN_QUAD_VERTEX_SHADER;
    extern const char* SCREEN_QUAD_FRAGMENT_SHADER;
    extern const char* OUTLINE_VERTEX_SHADER;
    extern const char* OUTLINE_FRAGMENT_SHADER;
    extern const char* NODE_VERTEX_SHADER;
    extern const char* NODE_FRAGMENT_SHADER;
    extern const char* SKYBOX_VERTEX_SHADER;
    extern const char* SKYBOX_FRAGMENT_SHADER;
    extern const char* QUAD2D_VERTEX_SHADER;
    extern const char* QUAD2D_FRAGMENT_SHADER;
    extern const char* QUAD3D_VERTEX_SHADER;
    extern const char* QUAD3D_FRAGMENT_SHADER;
#ifdef NINE_MORRIS_3D_DEBUG
    extern const char* ORIGIN_VERTEX_SHADER;
    extern const char* ORIGIN_FRAGMENT_SHADER;
#endif
    extern const char* TEXT_VERTEX_SHADER;
    extern const char* TEXT_FRAGMENT_SHADER;

    extern const char* ICON_512;
    extern const char* ICON_256;
    extern const char* ICON_128;
    extern const char* ICON_64;
    extern const char* ICON_32;

    extern const char* ARROW_CURSOR;
    extern const char* CROSS_CURSOR;
}
