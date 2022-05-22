#include <fstream>
#include <iomanip>
#include <string>

#include <nlohmann/json.hpp>

#include "application/platform.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/paths.h"
#include "other/logging.h"
#include "other/user_data.h"

#define OPTIONS_FILE "options.json"

using json = nlohmann::json;

namespace options {
    void save_options_to_file(const Options& options) noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_save_and_options(OPTIONS_FILE);
        } catch (const user_data::UserNameError& e) {
            throw OptionsFileError(e.what());
        }

        std::ofstream file (file_path, std::ios::trunc);

        if (!file.is_open()) {
            std::string message = "Could not open options file '" + file_path + "' for writing";
            throw OptionsFileNotOpenError(message);
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

        file << std::setw(4) << object;

        DEB_INFO("Saved options to file '{}'", file_path);
    }

    void load_options_from_file(Options& options) noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_save_and_options(OPTIONS_FILE);
        } catch (const user_data::UserNameError& e) {
            throw OptionsFileError(e.what());
        }

        std::ifstream file (file_path);

        if (!file.is_open()) {
            std::string message = "Could not open options file '" + file_path + "'";
            throw OptionsFileNotOpenError(message);
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

        std::string texture_quality;
        int samples;
        int anisotropic_filtering;
        bool vsync;
        bool save_on_exit;
        std::string skybox;
        bool custom_cursor;
        float sensitivity;

        try {
            texture_quality = object.at("texture_quality").get<std::string>();
            samples = object.at("samples").get<int>();
            anisotropic_filtering = object.at("anisotropic_filtering").get<int>();
            vsync = object.at("vsync").get<bool>();
            save_on_exit = object.at("save_on_exit").get<bool>();
            skybox = object.at("skybox").get<std::string>();
            custom_cursor = object.at("custom_cursor").get<bool>();
            sensitivity = object.at("sensitivity").get<float>();
        } catch (const json::out_of_range& e) {
            throw OptionsFileError(e.what());
        } catch (const json::type_error& e) {
            throw OptionsFileError(e.what());
        } catch (const json::exception& e) {
            throw OptionsFileError(e.what());
        }

        if (texture_quality != NORMAL && texture_quality != LOW) {
            throw OptionsFileError("Options file is invalid: texture_quality");
        }

        if (samples != 1 && samples != 2 && samples != 4) {
            throw OptionsFileError("Options file is invalid: samples");
        }

        if (anisotropic_filtering != 0 && anisotropic_filtering != 4 && anisotropic_filtering != 8) {
            throw OptionsFileError("Options file is invalid: anisotropic_filtering");
        }

        if (skybox != FIELD && skybox != AUTUMN) {
            throw OptionsFileError("Options file is invalid: skybox");
        }

        if (sensitivity < 0.5f || sensitivity > 2.0f) {
            throw OptionsFileError("Options file is invalid: sensitivity");
        }

        options.texture_quality = texture_quality;
        options.samples = samples;
        options.anisotropic_filtering = anisotropic_filtering;
        options.vsync = vsync;
        options.save_on_exit = save_on_exit;
        options.skybox = skybox;
        options.custom_cursor = custom_cursor;
        options.sensitivity = sensitivity;

        DEB_INFO("Loaded options from file '{}'", file_path);
    }

    void create_options_file() noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_save_and_options(OPTIONS_FILE);
        } catch (const user_data::UserNameError& e) {
            throw OptionsFileError(e.what());
        }

        std::ofstream file (file_path);

        if (!file.is_open()) {
            std::string message = "Could not open options file '" + file_path + "' for writing";
            throw OptionsFileNotOpenError(message);
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
