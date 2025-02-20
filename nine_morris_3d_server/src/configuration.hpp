#pragma once

#include <chrono>
#include <limits>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <cstdint>

#include <cereal/cereal.hpp>
#include <cereal/types/chrono.hpp>

#include "version.hpp"

struct Configuration {
    std::uint16_t port {7915};
    std::uint32_t max_clients {std::numeric_limits<std::uint16_t>::max()};
    std::chrono::seconds session_collect_period {std::chrono::seconds(15)};
    std::chrono::seconds connection_check_period {std::chrono::seconds(10)};
    std::string log_target {"file"};
    std::string log_level {"info"};

    template<typename Archive>
    void serialize(Archive& archive, const std::uint32_t) {
        archive(
            CEREAL_NVP(port),
            CEREAL_NVP(max_clients),
            CEREAL_NVP(session_collect_period),
            CEREAL_NVP(connection_check_period),
            CEREAL_NVP(log_target),
            CEREAL_NVP(log_level)
        );
    }
};

CEREAL_CLASS_VERSION(Configuration, version_number())

void read_configuration(Configuration& configuration, const std::filesystem::path& file_path);
void write_configuration(const Configuration& configuration, const std::filesystem::path& file_path);

// Check if the file path exists and if not, create the directory file
void make_configuration_directory(const std::filesystem::path& file_path);

struct ConfigurationError : std::runtime_error {
    explicit ConfigurationError(const char* message)
        : std::runtime_error(message) {}
    explicit ConfigurationError(const std::string& message)
        : std::runtime_error(message) {}
};
