#pragma once

#include <stdexcept>
#include <string>
#include <filesystem>

#include <nine_morris_3d_engine/external/cereal.h++>

#include "version.hpp"

inline constexpr const char* OPTIONS_FILE_NAME {"options.dat"};

enum GameType : int {
    GameTypeLocal,
    GameTypeLocalVsComputer,
    GameTypeOnline
};

enum GameMode : int {
    GameModeNineMensMorris,
    GameModeTwelveMensMorris
};

enum Skybox : int {
    SkyboxNone,
    SkyboxField,
    SkyboxAutumn,
    SkyboxSummer,
    SkyboxNight,
    SkyboxSunset,
    SkyboxSky
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

enum Language : int {
    LanguageEnglish,
    LanguageRomanian
};

// Structure representing data saved and loaded from disk
struct Options {
    int game_type {GameTypeLocal};
    int game_mode {GameModeNineMensMorris};
    float audio_volume {1.0f};
    int skybox {SkyboxField};
    int anti_aliasing {AntiAliasing2x};
    int anisotropic_filtering {AnisotropicFiltering4x};
    int texture_quality {TextureQualityFull};
    int shadow_quality {ShadowQualityFull};
    int scale {Scale100};
    int language {LanguageEnglish};
    float camera_sensitivity {1.0f};
    char name[15 + 1] {};
    char address[253 + 1] {};
    char port[5 + 1] {};
    bool default_address_port {true};
    bool vsync {true};

    template<typename Archive>
    void serialize(Archive& archive, const std::uint32_t) {
        archive(
            game_type,
            game_mode,
            audio_volume,
            skybox,
            anti_aliasing,
            anisotropic_filtering,
            texture_quality,
            shadow_quality,
            scale,
            language,
            camera_sensitivity,
            name,
            address,
            port,
            default_address_port,
            vsync
        );
    }
};

CEREAL_CLASS_VERSION(Options, version_number())

void load_options(Options& options, const std::filesystem::path& file_path);
void save_options(const Options& options, const std::filesystem::path& file_path);

// Error thrown by load and save operations
struct OptionsError : std::runtime_error {
    explicit OptionsError(const char* message)
        : std::runtime_error(message) {}
    explicit OptionsError(const std::string& message)
        : std::runtime_error(message) {}
};
