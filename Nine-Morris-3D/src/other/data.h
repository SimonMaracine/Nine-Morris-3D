#pragma once

#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>

#include "game/game_options.h"
#include "launcher/launcher_options.h"
#include "other/constants.h"

struct Data {
    launcher_options::LauncherOptions launcher_options;
    game_options::GameOptions options;

    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;

    ImFont* imgui_info_font = nullptr;
    ImFont* imgui_windows_font = nullptr;

    std::unordered_map<Index, identifier::Id> node_ids;
    std::unordered_map<Index, identifier::Id> piece_ids;

    std::shared_ptr<music::MusicTrack> current_music_track;

    // Some ImGui widget options for radio buttons
    struct {
        int skybox = 0;
        int labeled_board = 0;
        int scene = 0;
    } imgui_option;
};
