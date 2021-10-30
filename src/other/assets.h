#pragma once

namespace assets {
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
    extern const char* WHITE_PIECE_TEXTURE;
    extern const char* BLACK_PIECE_TEXTURE;
    extern const char* SKYBOX_PX_TEXTURE;
    extern const char* SKYBOX_NX_TEXTURE;
    extern const char* SKYBOX_PY_TEXTURE;
    extern const char* SKYBOX_NY_TEXTURE;
    extern const char* SKYBOX_PZ_TEXTURE;
    extern const char* SKYBOX_NZ_TEXTURE;
}

// #include <string>
// #include <unordered_map>

// #include "other/model.h"
// #include "other/texture_data.h"

// using namespace model;

// enum class AssetType {
//     Mesh,
//     MeshP,
//     Texture,
//     TextureFlipped,
//     Sound
// };

// class AssetManager {
// public:
//     AssetManager() = default;
//     ~AssetManager() = default;

//     struct Asset {
//         AssetType type;
//         std::string file_path;
//     };

//     void define_asset(unsigned int id, AssetType type, const std::string& file_path);

//     std::string get_file_path(unsigned int id);

//     Rc<Mesh<Vertex>> get_mesh(unsigned int id);
//     Rc<Mesh<VertexP>> get_mesh_p(unsigned int id);
//     Rc<TextureData> get_texture(unsigned int id);
//     Rc<TextureData> get_texture_flipped(unsigned int id);

//     void load_now(unsigned int id, AssetType type);
//     void drop(unsigned int id, AssetType type);

//     void require(unsigned int id);
//     std::unordered_map<unsigned int, Asset> get_required();

//     // TODO sounds, music ...
// private:
//     struct Container {  // TODO maybe not needed
//         std::unordered_map<unsigned int, Rc<Mesh<Vertex>>> meshes;
//         std::unordered_map<unsigned int, Rc<Mesh<VertexP>>> meshes_p;
//         std::unordered_map<unsigned int, Rc<TextureData>> textures;
//         std::unordered_map<unsigned int, Rc<TextureData>> textures_flipped;
//     };

//     std::unordered_map<unsigned int, Asset> asset_declarations;
//     Container all_assets;

//     std::unordered_map<unsigned int, Asset> required;

//     friend class Loader;
// };
