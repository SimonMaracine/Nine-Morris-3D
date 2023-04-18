#pragma once

#include <engine/public/audio.h>
#include <engine/public/graphics.h>
#include <engine/public/external/imgui.h>

#include "game/game_options.h"
#include "launcher/launcher_options.h"
#include "other/constants.h"

struct Data {
    launcher_options::LauncherOptions launcher_options;
    game_options::GameOptions options;

    ImFont* imgui_info_font = nullptr;
    ImFont* imgui_windows_font = nullptr;

    std::unordered_map<size_t, sm::Identifier::Id> node_ids;
    std::unordered_map<size_t, sm::Identifier::Id> piece_ids;

    std::shared_ptr<sm::music::MusicTrack> current_music_track;

    // Some Dear ImGui widget options for radio buttons
    struct {
        int skybox = 0;
        bool labeled_board = false;
        int scene = 0;
    } imgui_option;
};
