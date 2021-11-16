#include <string>

#include "other/assets.h"

namespace assets {
    std::string path(const char* file_path) {
#ifndef NDEBUG
        // Use relative path for both operating systems
        return std::string(file_path);
#else
    #if defined(__GNUG__)
        std::string path = "/usr/share/ninemorris3d/";
        path.append(file_path);
        return path;
    #elif defined(_MSC_VER)
        // Just use relative path
        return std::string(file_path)
    #else
        #error "GCC or MSVC must be used (for now)"
    #endif
#endif
    }

    const char* BOARD_MESH = "data/models/board/board.obj";
    const char* BOARD_PAINT_MESH = "data/models/board/board_paint.obj";
    const char* NODE_MESH = "data/models/node/node.obj";
    const char* WHITE_PIECE_MESH = "data/models/piece/white_piece.obj";
    const char* BLACK_PIECE_MESH = "data/models/piece/black_piece.obj";
    const char* BOARD_TEXTURE = "data/textures/board/board_wood.png";
    const char* BOARD_PAINT_TEXTURE = "data/textures/board/board_paint.png";
    const char* WHITE_INDICATOR_TEXTURE = "data/textures/indicator/white_indicator.png";
    const char* BLACK_INDICATOR_TEXTURE = "data/textures/indicator/black_indicator.png";
    const char* LOADING_TEXTURE = "data/textures/loading/loading.png";
    const char* OPEN_SANS_FONT = "data/fonts/OpenSans-Semibold.ttf";
    const char* WHITE_PIECE_TEXTURE = "data/textures/piece/white_piece.png";
    const char* BLACK_PIECE_TEXTURE = "data/textures/piece/black_piece.png";
    const char* SKYBOX_PX_TEXTURE = "data/textures/skybox/field/px.png";
    const char* SKYBOX_NX_TEXTURE = "data/textures/skybox/field/nx.png";
    const char* SKYBOX_PY_TEXTURE = "data/textures/skybox/field/py.png";
    const char* SKYBOX_NY_TEXTURE = "data/textures/skybox/field/ny.png";
    const char* SKYBOX_PZ_TEXTURE = "data/textures/skybox/field/pz.png";
    const char* SKYBOX_NZ_TEXTURE = "data/textures/skybox/field/nz.png";

    const char* BOARD_VERTEX_SHADER = "data/shaders/board.vert";
    const char* BOARD_FRAGMENT_SHADER = "data/shaders/board.frag";
    const char* BOARD_PAINT_VERTEX_SHADER = "data/shaders/board_paint.vert";
    const char* BOARD_PAINT_FRAGMENT_SHADER = "data/shaders/board_paint.frag";
    const char* PIECE_VERTEX_SHADER = "data/shaders/piece.vert";
    const char* PIECE_FRAGMENT_SHADER = "data/shaders/piece.frag";
    const char* SHADOW_VERTEX_SHADER = "data/shaders/shadow.vert";
    const char* SHADOW_FRAGMENT_SHADER = "data/shaders/shadow.frag";
    const char* SCREEN_QUAD_VERTEX_SHADER = "data/shaders/screen_quad.vert";
    const char* SCREEN_QUAD_FRAGMENT_SHADER = "data/shaders/screen_quad.frag";
    const char* OUTLINE_VERTEX_SHADER = "data/shaders/outline.vert";
    const char* OUTLINE_FRAGMENT_SHADER = "data/shaders/outline.frag";
    const char* NODE_VERTEX_SHADER = "data/shaders/node.vert";
    const char* NODE_FRAGMENT_SHADER = "data/shaders/node.frag";
    const char* SKYBOX_VERTEX_SHADER = "data/shaders/skybox.vert";
    const char* SKYBOX_FRAGMENT_SHADER = "data/shaders/skybox.frag";
    const char* QUAD2D_VERTEX_SHADER = "data/shaders/quad2d.vert";
    const char* QUAD2D_FRAGMENT_SHADER = "data/shaders/quad2d.frag";
    const char* QUAD3D_VERTEX_SHADER = "data/shaders/quad3d.vert";
    const char* QUAD3D_FRAGMENT_SHADER = "data/shaders/quad3d.frag";
#ifndef NDEBUG
    const char* ORIGIN_VERTEX_SHADER = "data/shaders/origin.vert";
    const char* ORIGIN_FRAGMENT_SHADER = "data/shaders/origin.frag";
#endif
}
