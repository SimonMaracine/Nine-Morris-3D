#include <fstream>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "nine_morris_3d/options.h"
#include "other/logging.h"
#include "other/user_data.h"

#define OPTIONS_FILE "options.json"

using json = nlohmann::json;

namespace options {
    void load_options_from_file(Options& options) {
        std::string path = user_data::get_user_data_path() + "/" + OPTIONS_FILE;

        std::ifstream file = std::ifstream(path, std::ios::in);

        if (!file.is_open()) {
            spdlog::error("Could not open options file '{}'", path.c_str());
            return;
        }

        json object;

        std::string contents, line;

        while (std::getline(file, line)) {
            contents += line;
            contents += '\n';
        }

        try {
            object = json::parse(contents.c_str());
        } catch (json::parse_error& e) {
            spdlog::error("{}", e.what());
            return;
        }

        // int texture_quality;
        int samples;
        bool vsync;
        bool save_on_exit;

        try {
            // texture_quality = object.at("texture_quality").get<int>();
            samples = object.at("samples").get<int>();
            vsync = object.at("vsync").get<bool>();
            save_on_exit = object.at("save_on_exit").get<bool>();
        } catch (json::out_of_range& e) {
            spdlog::error("{}", e.what());
            return;
        } catch (json::type_error& e) {
            spdlog::error("{}", e.what());
            return;
        } catch (json::exception& e) {
            spdlog::error("{}", e.what());
            return;
        }

        if (samples < 1 || samples > 4) {
            spdlog::error("Options file is wrong: samples");
            return;
        }
        // if (!(texture_quality == 0 || texture_quality == 1)) {
        //     spdlog::error("Options file is wrong: texture_quality");
        //     return;
        // }

        // options.texture_quality = texture_quality;
        options.samples = samples;
        options.vsync = vsync;
        options.save_on_exit = save_on_exit;

        SPDLOG_INFO("Loaded options from file '{}'", OPTIONS_FILE);
    }

    void save_options_to_file(const Options& options) {
        std::string path = user_data::get_user_data_path() + "/" + OPTIONS_FILE;

        std::ofstream file = std::ofstream(path.c_str(), std::ios::out | std::ios::trunc);

        if (!file.is_open()) {
            spdlog::error("Could not open options file '{}' for writing", path.c_str());

            if (!user_data::user_data_directory_exists()) {
                spdlog::info("User data folder missing; creating one...");

                if (!user_data::create_user_data_directory()) {
                    spdlog::error("Could not create user data directory");
                } else {
                    create_options_file();
                }
            }

            return;
        }

        json object;
        // object["texture_quality"] = options.texture_quality;
        object["samples"] = options.samples;
        object["vsync"] = options.vsync;
        object["save_on_exit"] = options.save_on_exit;

        file << std::setw(4) << object;
        file.close();

        SPDLOG_INFO("Saved options to file '{}'", path.c_str());
    }

    void create_options_file() {
        std::string path = user_data::get_user_data_path() + "/" + OPTIONS_FILE;

        std::ofstream file = std::ofstream(path.c_str(), std::ios::out);

        if (!file.is_open()) {
            spdlog::error("Could not open options file '{}' for writing", path.c_str());
            return;
        }

        file << (
            "{\n"
                "\"samples\": 2,\n"
                "\"save_on_exit\": true,\n"
                "\"vsync\": true\n"
            "{"
        );

        file.close();

        SPDLOG_INFO("Created options file '{}'", path.c_str());
    }
}
