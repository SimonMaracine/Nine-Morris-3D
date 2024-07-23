#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/resmanager.h++>

struct HeavyResources {
    resmanager::Cache<sm::TextureData> texture_data;
    resmanager::Cache<sm::Mesh> mesh;
    resmanager::Cache<sm::SoundData> sound_data;
};
