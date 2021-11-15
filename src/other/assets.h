#pragma once

#include <string>

namespace assets {
    std::string path(const char* file_path);

    extern const char* BOARD_MESH;
    extern const char* BOARD_PAINT_MESH;
    extern const char* NODE_MESH;
    extern const char* WHITE_PIECE_MESH;
    extern const char* BLACK_PIECE_MESH;
    extern const char* BOARD_TEXTURE;
    extern const char* BOARD_PAINT_TEXTURE;
    extern const char* WHITE_INDICATOR_TEXTURE;
    extern const char* BLACK_INDICATOR_TEXTURE;
    extern const char* LOADING_TEXTURE;
    extern const char* OPEN_SANS_FONT;
    extern const char* WHITE_PIECE_TEXTURE;
    extern const char* BLACK_PIECE_TEXTURE;
    extern const char* SKYBOX_PX_TEXTURE;
    extern const char* SKYBOX_NX_TEXTURE;
    extern const char* SKYBOX_PY_TEXTURE;
    extern const char* SKYBOX_NY_TEXTURE;
    extern const char* SKYBOX_PZ_TEXTURE;
    extern const char* SKYBOX_NZ_TEXTURE;

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
#ifndef NDEBUG
    extern const char* ORIGIN_VERTEX_SHADER;
    extern const char* ORIGIN_FRAGMENT_SHADER;
#endif
}
