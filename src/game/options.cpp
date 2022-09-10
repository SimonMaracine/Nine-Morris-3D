#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/options.h"

#define OPTIONS_FILE "options.json"

namespace options {
    template<typename Archive>
    void serialize(Archive& archive, Options& options) {
        archive(
            options.samples,
            options.anisotropic_filtering,
            options.vsync,
            options.save_on_exit,
            options.skybox,
            options.custom_cursor,
            options.sensitivity,
            options.hide_timer,
            options.labeled_board,
            options.white_player,
            options.black_player
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

    void save_options_to_file(const Options& options) noexcept(false) {
        const std::string file_path = get_file_path();

        std::ofstream file {file_path, std::ios::trunc};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "' for writing"
            );
        }

        try {
            cereal::JSONOutputArchive output {file};
            output(options);
        } catch (const std::exception& e) {  // Just to be sure...
            throw OptionsFileError(e.what());
        }

        DEB_INFO("Saved options to file '{}'", file_path);
    }

    void load_options_from_file(Options& options) noexcept(false) {
        const std::string file_path = get_file_path();

        std::ifstream file {file_path};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "'"
            );
        }

        Options temporary;

        try {
            cereal::JSONInputArchive input {file};
            input(temporary);
        } catch (const cereal::Exception& e) {
            throw OptionsFileError(e.what());
        } catch (const std::exception& e) {
            throw OptionsFileError(e.what());
        }

        if (temporary.samples != 1 && temporary.samples != 2 && temporary.samples != 4) {
            throw OptionsFileError("Options file is invalid: samples");
        }

        if (temporary.anisotropic_filtering != 0 && temporary.anisotropic_filtering != 4
                && temporary.anisotropic_filtering != 8) {
            throw OptionsFileError("Options file is invalid: anisotropic_filtering");
        }

        if (temporary.skybox != FIELD && temporary.skybox != AUTUMN) {
            throw OptionsFileError("Options file is invalid: skybox");
        }

        if (temporary.sensitivity < 0.5f || temporary.sensitivity > 2.0f) {
            throw OptionsFileError("Options file is invalid: sensitivity");
        }

        if (temporary.white_player != HUMAN || temporary.white_player != COMPUTER) {
            throw OptionsFileError("Options file is invalid: white_player");
        }

        if (temporary.black_player != HUMAN || temporary.black_player != COMPUTER) {
            throw OptionsFileError("Options file is invalid: black_player");
        }

        options = temporary;

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

        Options options;

        try {
            cereal::JSONOutputArchive output {file};
            output(options);
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
}
