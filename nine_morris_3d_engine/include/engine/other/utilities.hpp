#pragma once

#include <string>
#include <optional>

namespace sm {
    struct Utils {
        static std::string get_file_name(const std::string& file_path);
        static std::string get_directory_name(const std::string& file_path);
        static std::optional<std::string> read_file(const std::string& file_path);
        static std::string get_information();
    };
}
