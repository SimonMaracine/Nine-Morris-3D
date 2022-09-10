#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/components/options.h"
#include "game/systems/options.h"
#include "save_load.h"
// #include "other/paths.h"
// #include "other/logging.h"
// #include "other/user_data.h"

#define OPTIONS_FILE "options.json"

template<typename Archive>
void serialize(Archive& archive, OptionsComponent& options_c) {
    archive(
        options_c.black_player  // FIXME see which fields remain and which not
        // options_c.texture_quality, options_c.samples,
        // options_c.anisotropic_filtering, options_c.vsync, options_c.save_on_exit,
        // options_c.skybox, options_c.custom_cursor, options_c.sensitivity,
        // options_c.hide_timer, options_c.labeled_board, options_c.normal_mapping,
        // options_c.bloom, options_c.bloom_strength, options_c.white_player,
        // options_c.black_player
    );
}

static std::string get_file_path() noexcept(false) {
    std::string file_path;

    try {
        file_path = paths::path_for_saved_data(OPTIONS_FILE);
    } catch (const user_data::UserNameError& e) {
        throw OptionsFileError(e.what());
    }

    return file_path;
}

void save_options_system(entt::registry& registry) noexcept(false) {
    const std::string file_path = get_file_path();

    std::ofstream file {file_path, std::ios::trunc};

    if (!file.is_open()) {
        throw OptionsFileNotOpenError(
            "Could not open options file '" + file_path + "' for writing"
        );
    }

    auto view = registry.view<OptionsComponent>();
    auto& options_c = view.get<OptionsComponent>(view.back());

    try {
        cereal::JSONOutputArchive output {file};
        output(options_c);
    } catch (const std::exception& e) {  // Just to be sure...
        throw OptionsFileError(e.what());
    }

    DEB_INFO("Saved options to file '{}'", file_path);
}

void load_options_system(entt::registry& registry) noexcept(false) {
    const std::string file_path = get_file_path();

    std::ifstream file {file_path};

    if (!file.is_open()) {
        throw OptionsFileNotOpenError(
            "Could not open options file '" + file_path + "'"
        );
    }

    auto view = registry.view<OptionsComponent>();
    auto& options_c = view.get<OptionsComponent>(view.back());

    OptionsComponent temporary;

    try {
        cereal::JSONInputArchive input {file};
        input(temporary);
    } catch (const cereal::Exception& e) {
        throw save_load::SaveFileError(e.what());
    } catch (const std::exception& e) {
        throw save_load::SaveFileError(e.what());
    }

    // FIXME see which fields remain and which not

    // if (options_c.texture_quality != NORMAL && options_c.texture_quality != LOW) {
    //     throw OptionsFileError("Options file is invalid: texture_quality");
    // }

    // if (options_c.samples != 1 && options_c.samples != 2 && options_c.samples != 4) {
    //     throw OptionsFileError("Options file is invalid: samples");
    // }

    // if (options_c.anisotropic_filtering != 0 && options_c.anisotropic_filtering != 4
    //         && options_c.anisotropic_filtering != 8) {
    //     throw OptionsFileError("Options file is invalid: anisotropic_filtering");
    // }

    // if (options_c.skybox != FIELD && options_c.skybox != AUTUMN) {
    //     throw OptionsFileError("Options file is invalid: skybox");
    // }

    // if (options_c.sensitivity < 0.5f || options_c.sensitivity > 2.0f) {
    //     throw OptionsFileError("Options file is invalid: sensitivity");
    // }

    // if (options_c.bloom_strength < 0.1f || options_c.bloom_strength > 1.0f) {
    //     throw OptionsFileError("Options file is invalid: bloom_strength");
    // }

    // if (options_c.white_player != HUMAN || options_c.white_player != COMPUTER) {
    //     throw OptionsFileError("Options file is invalid: white_player");
    // }

    // if (options_c.black_player != HUMAN || options_c.black_player != COMPUTER) {
    //     throw OptionsFileError("Options file is invalid: black_player");
    // }

    options_c = temporary;

    DEB_INFO("Loaded options from file '{}'", file_path);
}

void create_options_file() noexcept(false) {
    const std::string file_path = get_file_path();

    std::ofstream file {file_path, std::ios::trunc};

    if (!file.is_open()) {
        throw OptionsFileNotOpenError(
            "Could not open options file '" + file_path + "' for writing"
        );
    }

    OptionsComponent options_c;

    try {
        cereal::JSONOutputArchive output {file};
        output(options_c);
    } catch (const std::exception& e) {  // Just to be sure...
        throw OptionsFileError(e.what());
    }

    DEB_INFO("Created options file '{}'", file_path);
}

void handle_options_file_not_open_error(std::string_view app_name) {
    bool user_data_directory;

    try {
        user_data_directory = user_data::user_data_directory_exists(app_name);
    } catch (const user_data::UserNameError& e) {
        REL_ERROR("{}", e.what());
        return;
    }

    if (!user_data_directory) {
        REL_INFO("User data folder missing; creating one...");

        try {
            const bool success = user_data::create_user_data_directory(app_name);
            if (success) {
                try {
                    create_options_file();

                    REL_INFO("Recreated options file");
                } catch (const OptionsFileNotOpenError& e) {
                    REL_ERROR("{}", e.what());
                    return;
                } catch (const OptionsFileError& e) {
                    REL_ERROR("{}", e.what());
                    return;
                }
            } else {
                REL_ERROR("Could not create user data directory");
                return;
            }
        } catch (const user_data::UserNameError& e) {
            REL_ERROR("{}", e.what());
            return;
        }
    }
}
