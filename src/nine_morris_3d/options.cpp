#include <fstream>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "other/logging.h"
#include "nine_morris_3d/options.h"

#define OPTIONS_FILE "options.json"

using json = nlohmann::json;

namespace options {
    void load_options_from_file(Options& options) {
        std::ifstream file = std::ifstream(OPTIONS_FILE, std::ios::in | std::ios::ate);

        if (!file.is_open()) {
            spdlog::error("Could not open options file '{}'", OPTIONS_FILE);
            return;
        }

        std::size_t size = (std::size_t) file.tellg();
        char* buffer = new char[size + 1];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        buffer[size] = 0;

        json object;

        try {
            object = json::parse(buffer);
        } catch (json::parse_error& e) {
            spdlog::error("{}", e.what());
            delete[] buffer;
            return;
        }

        delete[] buffer;

        int texture_quality;
        int samples;
        bool vsync;
        bool save_on_exit;

        try {
            texture_quality = object.at("texture_quality").get<int>();
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
        if (!(texture_quality == 0 || texture_quality == 1)) {
            spdlog::error("Options file is wrong: texture_quality");
            return;
        }

        options.vsync = vsync;
        options.samples = samples;
        options.texture_quality = texture_quality;
        options.save_on_exit = save_on_exit;

        SPDLOG_INFO("Loaded options from file '{}'", OPTIONS_FILE);
    }

    void save_options_to_file(const Options& options) {
        std::ofstream file = std::ofstream(OPTIONS_FILE, std::ios::out | std::ios::trunc);

        json object;
        object["texture_quality"] = options.texture_quality;
        object["samples"] = options.samples;
        object["vsync"] = options.vsync;
        object["save_on_exit"] = options.save_on_exit;

        file << std::setw(4) << object;
        file.close();

        SPDLOG_INFO("Saved options to file '{}'", OPTIONS_FILE);
    }
}
