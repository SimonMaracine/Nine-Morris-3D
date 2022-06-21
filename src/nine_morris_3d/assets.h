#pragma once

namespace assets {
    void maybe_initialize_assets();

    extern std::string BOARD_MESH;
    extern std::string BOARD_PAINT_MESH;
    extern std::string NODE_MESH;
    extern std::string WHITE_PIECE_MESH;
    extern std::string BLACK_PIECE_MESH;
    extern std::string BOARD_WOOD_TEXTURE;
    extern std::string BOARD_PAINT_TEXTURE;
    extern std::string WHITE_PIECE_TEXTURE;
    extern std::string BLACK_PIECE_TEXTURE;
    extern std::string BOARD_WOOD_TEXTURE_SMALL;
    extern std::string BOARD_PAINT_TEXTURE_SMALL;
    extern std::string WHITE_PIECE_TEXTURE_SMALL;
    extern std::string BLACK_PIECE_TEXTURE_SMALL;
    extern std::string WHITE_INDICATOR_TEXTURE;
    extern std::string BLACK_INDICATOR_TEXTURE;
    extern std::string SPLASH_SCREEN_TEXTURE;
    extern std::string OPEN_SANS_FONT;
    extern std::string GOOD_DOG_PLAIN_FONT;

    extern std::string FIELD_PX_TEXTURE;
    extern std::string FIELD_NX_TEXTURE;
    extern std::string FIELD_PY_TEXTURE;
    extern std::string FIELD_NY_TEXTURE;
    extern std::string FIELD_PZ_TEXTURE;
    extern std::string FIELD_NZ_TEXTURE;
    extern std::string FIELD_PX_TEXTURE_SMALL;
    extern std::string FIELD_NX_TEXTURE_SMALL;
    extern std::string FIELD_PY_TEXTURE_SMALL;
    extern std::string FIELD_NY_TEXTURE_SMALL;
    extern std::string FIELD_PZ_TEXTURE_SMALL;
    extern std::string FIELD_NZ_TEXTURE_SMALL;
    extern std::string AUTUMN_PX_TEXTURE;
    extern std::string AUTUMN_NX_TEXTURE;
    extern std::string AUTUMN_PY_TEXTURE;
    extern std::string AUTUMN_NY_TEXTURE;
    extern std::string AUTUMN_PZ_TEXTURE;
    extern std::string AUTUMN_NZ_TEXTURE;
    extern std::string AUTUMN_PX_TEXTURE_SMALL;
    extern std::string AUTUMN_NX_TEXTURE_SMALL;
    extern std::string AUTUMN_PY_TEXTURE_SMALL;
    extern std::string AUTUMN_NY_TEXTURE_SMALL;
    extern std::string AUTUMN_PZ_TEXTURE_SMALL;
    extern std::string AUTUMN_NZ_TEXTURE_SMALL;

    extern std::string BOARD_VERTEX_SHADER;
    extern std::string BOARD_FRAGMENT_SHADER;
    extern std::string BOARD_PAINT_VERTEX_SHADER;
    extern std::string BOARD_PAINT_FRAGMENT_SHADER;
    extern std::string PIECE_VERTEX_SHADER;
    extern std::string PIECE_FRAGMENT_SHADER;
    extern std::string NODE_VERTEX_SHADER;
    extern std::string NODE_FRAGMENT_SHADER;

    extern std::string ICON_512;
    extern std::string ICON_256;
    extern std::string ICON_128;
    extern std::string ICON_64;
    extern std::string ICON_32;

    extern std::string ARROW_CURSOR;
    extern std::string CROSS_CURSOR;
}
