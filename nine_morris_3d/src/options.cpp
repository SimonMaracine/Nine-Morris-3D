#include "options.hpp"

#include <fstream>
#include <algorithm>

using namespace std::string_literals;

static void validate(Options& options) {
    switch (static_cast<GameType>(options.game_type)) {
        case GameTypeLocal:
        case GameTypeLocalVsComputer:
        case GameTypeOnline:
            break;
        default:
            goto corrupted;
    }

    switch (static_cast<GameMode>(options.game_mode)) {
        case GameModeNineMensMorris:
        case GameModeTwelveMensMorris:
            break;
        default:
            goto corrupted;
    }

    if (options.audio_volume < 0.0f || options.audio_volume > 1.0f) {
        goto corrupted;
    }

    switch (static_cast<Skybox>(options.skybox)) {
        case SkyboxNone:
        case SkyboxField:
        case SkyboxAutumn:
        case SkyboxSummer:
        case SkyboxNight:
        case SkyboxSunset:
        case SkyboxSky:
            break;
        default:
            goto corrupted;
    }

    switch (static_cast<AntiAliasing>(options.anti_aliasing)) {
        case AntiAliasingOff:
        case AntiAliasing4x:
        case AntiAliasing2x:
            break;
        default:
            goto corrupted;
    }

    switch (static_cast<AnisotropicFiltering>(options.anisotropic_filtering)) {
        case AnisotropicFilteringOff:
        case AnisotropicFiltering4x:
        case AnisotropicFiltering8x:
            break;
        default:
            goto corrupted;
    }

    switch (static_cast<TextureQuality>(options.texture_quality)) {
        case TextureQualityHalf:
        case TextureQualityFull:
            break;
        default:
            goto corrupted;
    }

    switch (static_cast<ShadowQuality>(options.shadow_quality)) {
        case ShadowQualityHalf:
        case ShadowQualityFull:
            break;
        default:
            goto corrupted;
    }

    switch (static_cast<Scale>(options.scale)) {
        case Scale100:
        case Scale200:
            break;
        default:
            goto corrupted;
    }

    switch (static_cast<Language>(options.language)) {
        case LanguageEnglish:
        case LanguageRomanian:
            break;
        default:
            goto corrupted;
    }

    if (options.camera_sensitivity < 0.5f || options.camera_sensitivity > 2.0f) {
        goto corrupted;
    }

    // Sanity check these
    options.name[sizeof(options.name) - 1] = 0;
    options.address[sizeof(options.address) - 1] = 0;
    options.port[sizeof(options.port) - 1] = 0;

    return;

corrupted:
    throw OptionsError("Data has been corrupted");
}

void load_options(Options& options, const std::filesystem::path& file_path) {
    std::ifstream stream {file_path, std::ios::binary};

    if (!stream.is_open()) {
        throw OptionsError("Could not open file for reading: `" + file_path.string() + "`");
    }

    try {
        cereal::BinaryInputArchive archive {stream};
        archive(options);
    } catch (const cereal::Exception& e) {
        options = Options();
        throw OptionsError("Error reading from file: "s + e.what());
    } catch (...) {
        options = Options();
        throw OptionsError("Unexpected error reading from file");
    }

    // Must reset the data on error, as it is corrupted

    validate(options);
}

void save_options(const Options& options, const std::filesystem::path& file_path) {
    std::ofstream stream {file_path, std::ios::binary};

    if (!stream.is_open()) {
        throw OptionsError("Could not open file for writing: `" + file_path.string() + "`");
    }

    try {
        cereal::BinaryOutputArchive archive {stream};
        archive(options);
    } catch (const cereal::Exception& e) {
        throw OptionsError("Error writing to file: "s + e.what());
    }
}
