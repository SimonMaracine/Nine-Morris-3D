#include <string>
#include <fstream>

#include <nlohmann/json.hpp>

#include "other/options.h"
#include "other/logging.h"

#define OPTIONS_FILE "options.json"

using json = nlohmann::json;

namespace options {
    void get_options_from_file(Options& options) {
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

        json object = json::parse(buffer, nullptr, false);

        delete[] buffer;

        if (object.is_discarded()) {
            spdlog::error("Options file is either corrupted or just invalid");
            return;
        }

        int vsync;
        int samples;
        int texture_quality;

        try {
            vsync = object.at("vsync").get<bool>();
            samples = object.at("samples").get<int>();
            texture_quality = object.at("texture_quality").get<int>();
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

        SPDLOG_INFO("Got options from file '{}'", OPTIONS_FILE);
    }
}
