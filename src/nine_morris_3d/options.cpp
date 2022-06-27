#include <nlohmann/json.hpp>

#include "application/platform.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/options.h"
#include "other/paths.h"
#include "other/logging.h"
#include "other/user_data.h"

#define OPTIONS_FILE "options.json"

using json = nlohmann::json;

namespace options {
    void save_options_to_file(const Options& options) noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_saved_data(OPTIONS_FILE);
        } catch (const user_data::UserNameError& e) {
            throw OptionsFileError(e.what());
        }

        std::ofstream file (file_path, std::ios::trunc);

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "' for writing"
            );
        }

        json object;

        object["texture_quality"] = options.texture_quality;
        object["samples"] = options.samples;
        object["anisotropic_filtering"] = options.anisotropic_filtering;
        object["vsync"] = options.vsync;
        object["save_on_exit"] = options.save_on_exit;
        object["skybox"] = options.skybox;
        object["custom_cursor"] = options.custom_cursor;
        object["sensitivity"] = options.sensitivity;
        object["hide_timer"] = options.hide_timer;
        object["labeled_board"] = options.labeled_board;
        object["normal_mapping"] = options.normal_mapping;

        file << std::setw(4) << object;

        DEB_INFO("Saved options to file '{}'", file_path);
    }

    void load_options_from_file(Options& options) noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_saved_data(OPTIONS_FILE);
        } catch (const user_data::UserNameError& e) {
            throw OptionsFileError(e.what());
        }

        std::ifstream file (file_path);

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "'"
            );
        }

        std::string contents, line;

        while (std::getline(file, line)) {
            contents += line;
            contents += '\n';
        }

        json object;

        try {
            object = json::parse(contents);
        } catch (const json::parse_error& e) {
            throw OptionsFileError(e.what());
        }

        Options options_file;

        try {
            options_file.texture_quality = object.at("texture_quality").get<std::string>();
            options_file.samples = object.at("samples").get<int>();
            options_file.anisotropic_filtering = object.at("anisotropic_filtering").get<int>();
            options_file.vsync = object.at("vsync").get<bool>();
            options_file.save_on_exit = object.at("save_on_exit").get<bool>();
            options_file.skybox = object.at("skybox").get<std::string>();
            options_file.custom_cursor = object.at("custom_cursor").get<bool>();
            options_file.sensitivity = object.at("sensitivity").get<float>();
            options_file.hide_timer = object.at("hide_timer").get<bool>();
            options_file.labeled_board = object.at("labeled_board").get<bool>();
            options_file.normal_mapping= object.at("normal_mapping").get<bool>();
        } catch (const json::out_of_range& e) {
            throw OptionsFileError(e.what());
        } catch (const json::type_error& e) {
            throw OptionsFileError(e.what());
        } catch (const json::exception& e) {
            throw OptionsFileError(e.what());
        }

        if (options_file.texture_quality != NORMAL && options_file.texture_quality != LOW) {
            throw OptionsFileError("Options file is invalid: texture_quality");
        }

        if (options_file.samples != 1 && options_file.samples != 2 && options_file.samples != 4) {
            throw OptionsFileError("Options file is invalid: samples");
        }

        if (options_file.anisotropic_filtering != 0 && options_file.anisotropic_filtering != 4
                && options_file.anisotropic_filtering != 8) {
            throw OptionsFileError("Options file is invalid: anisotropic_filtering");
        }

        if (options_file.skybox != FIELD && options_file.skybox != AUTUMN) {
            throw OptionsFileError("Options file is invalid: skybox");
        }

        if (options_file.sensitivity < 0.5f || options_file.sensitivity > 2.0f) {
            throw OptionsFileError("Options file is invalid: sensitivity");
        }

        options.texture_quality = std::move(options_file.texture_quality);
        options.samples = options_file.samples;
        options.anisotropic_filtering = options_file.anisotropic_filtering;
        options.vsync = options_file.vsync;
        options.save_on_exit = options_file.save_on_exit;
        options.skybox = std::move(options_file.skybox);
        options.custom_cursor = options_file.custom_cursor;
        options.sensitivity = options_file.sensitivity;
        options.hide_timer = options_file.hide_timer;
        options.labeled_board = options_file.labeled_board;
        options.normal_mapping = options_file.normal_mapping;

        DEB_INFO("Loaded options from file '{}'", file_path);
    }

    void create_options_file() noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_saved_data(OPTIONS_FILE);
        } catch (const user_data::UserNameError& e) {
            throw OptionsFileError(e.what());
        }

        std::ofstream file (file_path, std::ios::trunc);

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "' for writing"
            );
        }

        Options options;
        json object;

        object["texture_quality"] = options.texture_quality;
        object["samples"] = options.samples;
        object["anisotropic_filtering"] = options.anisotropic_filtering;
        object["vsync"] = options.vsync;
        object["save_on_exit"] = options.save_on_exit;
        object["skybox"] = options.skybox;
        object["custom_cursor"] = options.custom_cursor;
        object["sensitivity"] = options.sensitivity;
        object["hide_timer"] = options.hide_timer;
        object["labeled_board"] = options.labeled_board;
        object["normal_mapping"] = options.normal_mapping;

        file << object.dump(4);

        DEB_INFO("Created options file '{}'", file_path);
    }

    void handle_options_file_not_open_error() {
        bool user_data_directory;

        try {
            user_data_directory = user_data::user_data_directory_exists(APP_NAME);
        } catch (const user_data::UserNameError& e) {
            REL_ERROR("{}", e.what());
            return;
        }

        if (!user_data_directory) {
            REL_INFO("User data folder missing; creating one...");

            try {
                bool success = user_data::create_user_data_directory(APP_NAME);
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
