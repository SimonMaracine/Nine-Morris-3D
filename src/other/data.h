#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/game_options.h"
#include "launcher/launcher_options.h"

struct Data {
    launcher_options::LauncherOptions launcher_options;
    game_options::GameOptions options;

    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;

    ImFont* imgui_info_font = nullptr;
    ImFont* imgui_windows_font = nullptr;

    resmanager::Cache<Renderer::Model> model_cache;
    resmanager::Cache<Renderer::Quad> quad_cache;

    std::unordered_map<size_t, hover::Id> node_ids;
    std::unordered_map<size_t, hover::Id> piece_ids;
};
