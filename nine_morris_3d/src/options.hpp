#pragma once

#include <stdexcept>
#include <string>
#include <filesystem>

#include <nine_morris_3d_engine/external/cereal.h++>

#include "ver.hpp"

inline constexpr const char* OPTIONS_FILE_NAME {"options.dat"};

enum GameMode : int {
    GameModeNineMensMorris,
    GameModeTwelveMensMorris
};

enum Skybox : int {
    SkyboxNone,
    SkyboxField,
    SkyboxAutumn
};

enum AntiAliasing : int {
    AntiAliasingOff = 1,
    AntiAliasing2x = 2,
    AntiAliasing4x = 4
};

enum AnisotropicFiltering : int {
    AnisotropicFilteringOff = 0,
    AnisotropicFiltering4x = 4,
    AnisotropicFiltering8x = 8
};

enum TextureQuality : int {
    TextureQualityHalf,
    TextureQualityFull
};

enum ShadowQuality : int {
    ShadowQualityHalf = 2048,
    ShadowQualityFull = 4096
};

enum Scale : int {
    Scale100 = 1,
    Scale200 = 2
};

// Structure representing data saved and loaded from disk
struct Options {
    int game_mode {GameModeNineMensMorris};
    float master_volume {1.0f};
    float music_volume {0.7f};
    int skybox {SkyboxField};
    int anti_aliasing {AntiAliasing2x};
    int anisotropic_filtering {AnisotropicFiltering4x};
    int texture_quality {TextureQualityFull};
    int shadow_quality {ShadowQualityFull};
    int scale {Scale100};
    float camera_sensitivity {1.0f};
    char address[253] {};
    char port[5] {};  // Must be 5 bytes
    bool default_address_port {true};
    bool enable_music {false};
    bool vsync {true};
    bool custom_cursor {false};

    template<typename Archive>
    void serialize(Archive& archive, const std::uint32_t) {
        archive(
            game_mode,
            master_volume,
            music_volume,
            skybox,
            anti_aliasing,
            anisotropic_filtering,
            texture_quality,
            shadow_quality,
            scale,
            camera_sensitivity,
            address,
            port,
            default_address_port,
            enable_music,
            vsync,
            custom_cursor
        );
    }
};

CEREAL_CLASS_VERSION(Options, version_number())

void load_options(Options& options, const std::filesystem::path& file_path);  // TODO validation
void save_options(const Options& options, const std::filesystem::path& file_path);

// Error thrown by load and save operations
struct OptionsFileError : std::runtime_error {
    explicit OptionsFileError(const char* message)
        : std::runtime_error(message) {}
    explicit OptionsFileError(const std::string& message)
        : std::runtime_error(message) {}
};
