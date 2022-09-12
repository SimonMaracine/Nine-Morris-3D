#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/options.h"

struct NineMorris3D : public Application {
    NineMorris3D(const ApplicationBuilder& builder);
    virtual ~NineMorris3D();

    // void set_bloom(bool enable);
    // void set_bloom_strength(float strength);

    options::Options options;
    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;

    // std::shared_ptr<AssetsData> assets_data;  // FIXME this

    ImFont* imgui_info_font = nullptr;
    ImFont* imgui_windows_font = nullptr;

    hover::Id pieces_id[18] = { hover::null };
    hover::Id nodes_id[24] = { hover::null };
    hover::Id board_paint_id = hover::null;
};
