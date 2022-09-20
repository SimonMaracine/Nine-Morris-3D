#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/game_options.h"
#include "launcher/launcher_options.h"

struct Data {
    launcher_options::LauncherOptions launcher_options;
    game_options::GameOptions options;
    Resources res_thread;

    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;

    ImFont* imgui_info_font = nullptr;
    ImFont* imgui_windows_font = nullptr;

    hover::Id pieces_id[18] = { hover::null };
    hover::Id nodes_id[24] = { hover::null };
    hover::Id board_paint_id = hover::null;
};
