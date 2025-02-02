#include "configuration.hpp"

#include <fstream>
#include <algorithm>
#include <array>
#include <string_view>

#include <cereal/archives/json.hpp>

using namespace std::string_literals;
using namespace std::chrono_literals;
using namespace std::string_view_literals;

static constexpr std::array LOG_TARGETS {
    "none"sv,
    "console"sv,
    "file"sv,
    "both"sv
};

static constexpr std::array LOG_LEVELS {
    "trace"sv,
    "debug"sv,
    "info"sv,
    "warn"sv,
    "err"sv,
    "critical"sv,
    "off"sv
};

static void validate(Configuration& configuration) {
    configuration.session_collect_period = std::clamp(configuration.session_collect_period, 1s, 60s);
    configuration.connection_check_period = std::clamp(configuration.session_collect_period, 1s, 60s);

    if (std::find(LOG_TARGETS.begin(), LOG_TARGETS.end(), configuration.log_target) == LOG_TARGETS.end()) {
        configuration.log_level = "both";
    }

    if (std::find(LOG_LEVELS.begin(), LOG_LEVELS.end(), configuration.log_level) == LOG_LEVELS.end()) {
        configuration.log_level = "info";
    }
}

void load_configuration(Configuration& configuration, const std::filesystem::path& file_path) {
    std::ifstream stream {file_path};

    if (!stream.is_open()) {
        throw ConfigurationError("Could not open file for reading: `" + file_path.string() + "`");
    }

    try {
        cereal::JSONInputArchive archive {stream};
        archive(CEREAL_NVP(configuration));
    } catch (const cereal::Exception& e) {
        throw ConfigurationError("Error reading from file: "s + e.what());
    }

    validate(configuration);
}

void save_configuration(const Configuration& configuration, const std::filesystem::path& file_path) {
    std::ofstream stream {file_path};

    if (!stream.is_open()) {
        throw ConfigurationError("Could not open file for writing: `" + file_path.string() + "`");
    }

    try {
        cereal::JSONOutputArchive archive {stream};
        archive(CEREAL_NVP(configuration));
    } catch (const cereal::Exception& e) {
        throw ConfigurationError("Error writing to file: "s + e.what());
    }
}
