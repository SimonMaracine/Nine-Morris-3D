#pragma once

#include <game/constants.h>

struct OptionsComponent {
    // std::string texture_quality = NORMAL;
    // int samples = 2;
    // int anisotropic_filtering = 4;
    bool vsync = true;
    bool save_on_exit = true;
    std::string skybox = FIELD;
    bool custom_cursor = true;
    float sensitivity = 1.0f;
    bool hide_timer = false;
    bool labeled_board = true;
    // bool normal_mapping = true;
    // bool bloom = true;
    float bloom_strength = 0.3f;
    int white_player = HUMAN;
    int black_player = COMPUTER;
};

// TODO commented fields should be separate
