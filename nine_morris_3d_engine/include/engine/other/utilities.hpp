#pragma once

#include <string>
#include <optional>

namespace sm {
    namespace utils {
        std::optional<std::string> read_file(const std::string& file_path);
    };
}
