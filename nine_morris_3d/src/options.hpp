#pragma once

#include <stdexcept>
#include <string>
#include <filesystem>

#include <nine_morris_3d_engine/external/cereal.h++>

#include "ver.hpp"
#include "constants.hpp"

inline constexpr const char* OPTIONS_FILE_NAME {"options.dat"};

struct Options {
    int game_mode {static_cast<int>(GameMode::Standard)};
    int white_player {static_cast<int>(PlayerType::Human)};
    int black_player {static_cast<int>(PlayerType::Computer)};
    float master_volume {1.0f};
    float music_volume {0.7f};
    int skybox {static_cast<int>(Skybox::Field)};
    int anti_aliasing {static_cast<int>(AntiAliasing::_2x)};
    int anisotropic_filtering {static_cast<int>(AnisotropicFiltering::_4x)};
    int texture_quality {static_cast<int>(TextureQuality::Full)};
    int shadow_quality {static_cast<int>(ShadowQuality::Full)};
    int scale {static_cast<int>(Scale::_100)};
    float camera_sensitivity {1.0f};
    bool enable_music {false};
    bool vsync {true};
    bool custom_cursor {false};
    bool save_on_exit {false};
    bool hide_timer {false};
    bool hide_turn_indicator {false};
    bool labeled_board {true};

    template<typename Archive>
    void serialize(Archive& archive, const std::uint32_t) {
        archive(
            game_mode,
            white_player,
            black_player,
            master_volume,
            music_volume,
            skybox,
            anti_aliasing,
            anisotropic_filtering,
            texture_quality,
            shadow_quality,
            scale,
            camera_sensitivity,
            enable_music,
            vsync,
            custom_cursor,
            save_on_exit,
            hide_timer,
            hide_turn_indicator,
            labeled_board
        );
    }
};

CEREAL_CLASS_VERSION(Options, version_number())

void load_options(Options& options, const std::filesystem::path& file_path);  // TODO validation
void save_options(const Options& options, const std::filesystem::path& file_path);

struct OptionsFileError : std::runtime_error {
    explicit OptionsFileError(const char* message)
        : std::runtime_error(message) {}
    explicit OptionsFileError(const std::string& message)
        : std::runtime_error(message) {}
};
