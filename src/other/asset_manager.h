#pragma once

#include <string>
#include <unordered_map>

#include "other/model.h"
#include "other/texture_data.h"

using namespace model;

// class Loader;


enum class AssetType {
    FullModel,
    PositionModel,
    TextureData,
    Sound
};

class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager() = default;

    struct Asset {
        AssetType type;
        std::string file_path;
    };

    void define_asset(unsigned int id, AssetType type, const std::string& file_path);

    template<typename T>
    Rc<T> get(unsigned int id);

    void load_now(unsigned int id, AssetType type);
    void drop(unsigned int id, AssetType type);

    void require(unsigned int id);
    std::unordered_map<unsigned int, Asset> get_required();

    // TODO sounds, music ...
private:
    struct Container {
        std::unordered_map<unsigned int, Rc<Mesh<FullVertex>>> full_meshes;
        std::unordered_map<unsigned int, Rc<Mesh<PositionVertex>>> position_meshes;
        std::unordered_map<unsigned int, Rc<TextureData>> texture_datas;
    };

    std::unordered_map<unsigned int, Asset> asset_declarations;
    Container all_assets;

    std::unordered_map<unsigned int, Asset> required;

    friend class Loader;
};
