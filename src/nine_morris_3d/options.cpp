#include <fstream>
#include <exception>
#include <iomanip>
#include <string>

#include <nlohmann/json.hpp>

#include "nine_morris_3d/options.h"
#include "other/logging.h"
#include "other/user_data.h"

#define OPTIONS_FILE "options.json"

using json = nlohmann::json;

namespace options {
    static std::string path(const char* file) {  // Throws exception
#ifndef NDEBUG
        // Use relative path for both operating systems
        return std::string(file);
#else
    #if defined(__GNUG__)
        std::string path = user_data::get_user_data_path() + "/" + file + "/";
        return path;
    #elif defined(_MSC_VER)
        std::string path = user_data::get_user_data_path() + "\\" + file;
        return path;
    #else
        #error "GCC or MSVC must be used (for now)"
    #endif
#endif
    }

    void load_options_from_file(Options& options) {
        std::string file_path;
        try {
            file_path = path(OPTIONS_FILE);
        } catch (const std::runtime_error& e) {
            spdlog::error("{}", e.what());
            return;
        }

        std::ifstream file (file_path, std::ios::in);

        if (!file.is_open()) {
            spdlog::error("Could not open options file '{}'", file_path.c_str());
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
        } catch (const json::parse_error& e) {
            spdlog::error("{}", e.what());
            return;
        }

        std::string texture_quality;
        int samples;
        bool vsync;
        bool save_on_exit;

        try {
            texture_quality = object.at("texture_quality").get<std::string>();
            samples = object.at("samples").get<int>();
            vsync = object.at("vsync").get<bool>();
            save_on_exit = object.at("save_on_exit").get<bool>();
        } catch (const json::out_of_range& e) {
            spdlog::error("{}", e.what());
            return;
        } catch (const json::type_error& e) {
            spdlog::error("{}", e.what());
            return;
        } catch (const json::exception& e) {
            spdlog::error("{}", e.what());
            return;
        }

        if (samples != 1 && samples != 2 && samples != 4) {
            spdlog::error("Options file is wrong: samples");
            return;
        }
        if (texture_quality != "normal" && texture_quality != "low") {
            spdlog::error("Options file is wrong: texture_quality");
            return;
        }

        options.texture_quality = texture_quality;
        options.samples = samples;
        options.vsync = vsync;
        options.save_on_exit = save_on_exit;

        SPDLOG_INFO("Loaded options from file '{}'", file_path.c_str());
    }

    void save_options_to_file(const Options& options) {
        std::string file_path;
        try {
            file_path = path(OPTIONS_FILE);
        } catch (const std::runtime_error& e) {
            spdlog::error("{}", e.what());
            return;
        }

        std::ofstream file (file_path.c_str(), std::ios::out | std::ios::trunc);

        if (!file.is_open()) {
            spdlog::error("Could not open options file '{}' for writing", file_path.c_str());

            bool user_data_directory;

            try {
                user_data_directory = user_data::user_data_directory_exists();
            } catch (const std::runtime_error& e) {
                spdlog::error("{}", e.what());
                return;
            }

            if (!user_data_directory) {
                spdlog::info("User data folder missing; creating one...");

                try {
                    if (!user_data::create_user_data_directory()) {
                        spdlog::error("Could not create user data directory");
                    } else {
                        create_options_file();
                    }
                } catch (const std::runtime_error& e) {
                    spdlog::error("{}", e.what());
                    return;
                }
            }

            return;
        }

        json object;
        object["texture_quality"] = options.texture_quality;
        object["samples"] = options.samples;
        object["vsync"] = options.vsync;
        object["save_on_exit"] = options.save_on_exit;

        file << std::setw(4) << object;
        file.close();

        SPDLOG_INFO("Saved options to file '{}'", file_path.c_str());
    }

    void create_options_file() {
        std::string file_path;
        try {
            file_path = path(OPTIONS_FILE);
        } catch (const std::runtime_error& e) {
            spdlog::error("{}", e.what());
            return;
        }

        std::ofstream file (file_path.c_str(), std::ios::out);

        if (!file.is_open()) {
            spdlog::error("Could not open options file '{}' for writing", file_path.c_str());
            return;
        }

        file << (
            "{\n"
            "    \"samples\": 2,\n"
            "    \"save_on_exit\": true,\n"
            "    \"vsync\": true\n"
            "}"
        );

        file.close();

        SPDLOG_INFO("Created options file '{}'", file_path.c_str());
    }
}
