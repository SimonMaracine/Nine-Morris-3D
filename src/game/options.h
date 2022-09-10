#pragma once

#include "game/constants.h"

namespace options {
    // constexpr const char* NORMAL = "normal";  // TODO move this to launcher
    // constexpr const char* LOW = "low";

    constexpr const char* FIELD = "field";
    constexpr const char* AUTUMN = "autumn";

    constexpr int HUMAN = static_cast<int>(GamePlayer::Human);
    constexpr int COMPUTER = static_cast<int>(GamePlayer::Computer);

    class OptionsFileError : public std::runtime_error {
    public:
        OptionsFileError(const std::string& message)
            : std::runtime_error(message) {}
        OptionsFileError(const char* message)
            : std::runtime_error(message) {}
    };

    class OptionsFileNotOpenError : public OptionsFileError {
    public:
        OptionsFileNotOpenError(const std::string& message)
            : OptionsFileError(message) {}
        OptionsFileNotOpenError(const char* message)
            : OptionsFileError(message) {}
    };

    struct Options {
        // These are default values
        // std::string texture_quality = NORMAL;  // TODO move this to launcher
        int samples = 2;
        int anisotropic_filtering = 4;
        bool vsync = true;
        bool save_on_exit = true;
        std::string skybox = FIELD;
        bool custom_cursor = true;
        float sensitivity = 1.0f;
        bool hide_timer = false;
        bool labeled_board = true;
        // bool normal_mapping = true;  // TODO move this to launcher
        // bool bloom = true;
        // float bloom_strength = 0.3f;
        int white_player = HUMAN;
        int black_player = COMPUTER;
    };

    void save_options_to_file(const Options& options) noexcept(false);
    void load_options_from_file(Options& options) noexcept(false);

    void create_options_file() noexcept(false);
    void handle_options_file_not_open_error(std::string_view app_name);
}
