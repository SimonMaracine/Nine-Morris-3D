#pragma once

#include <string>
#include <unordered_map>

#include "opengl/renderer/texture.h"
#include "opengl/renderer/vertex_array.h"
#include "other/model.h"

#define CHECK_IDENTIFIER(id) \
    assert(full_meshes.find(id) == full_meshes.end()); \
    assert(position_meshes.find(id) == position_meshes.end()); \
    assert(vertex_arrays.find(id) == vertex_arrays.end()); \
    assert(texture_datas.find(id) == texture_datas.end()); \
    assert(textures.find(id) == textures.end()); \
    assert(textures_3d.find(id) == textures_3d.end());

using namespace model;

class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager() = default;

    // enum class Type {
    //     Model, VertexArray,
    //     TextureData, Texture, Texture3D
    // };

    void define_asset(unsigned int id, const std::string& file_path);

    void hold(unsigned int id, Rc<Mesh<FullVertex>> mesh);
    void hold(unsigned int id, Rc<Mesh<PositionVertex>> mesh);
    void hold(unsigned int id, Rc<VertexArray> vertex_array);
    void hold(unsigned int id, Rc<TextureData> texture_data);
    void hold(unsigned int id, Rc<Texture> texture);
    void hold(unsigned int id, Rc<Texture3D> texture_3d);

    void drop(unsigned int id);
    // TODO sounds, music ...
private:
    std::unordered_map<unsigned int, std::string> all_assets;

    std::unordered_map<unsigned int, Rc<Mesh<FullVertex>>> full_meshes;
    std::unordered_map<unsigned int, Rc<Mesh<PositionVertex>>> position_meshes;
    std::unordered_map<unsigned int, Rc<VertexArray>> vertex_arrays;

    std::unordered_map<unsigned int, Rc<TextureData>> texture_datas;
    std::unordered_map<unsigned int, Rc<Texture>> textures;
    std::unordered_map<unsigned int, Rc<Texture3D>> textures_3d;
};
