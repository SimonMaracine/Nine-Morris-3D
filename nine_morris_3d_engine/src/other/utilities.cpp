#include <string>
#include <optional>
#include <utility>
#include <cstddef>
#include <vector>
#include <cstring>
#include <fstream>

#include "engine/other/utilities.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    std::string Utils::get_file_name(const std::string& file_path) {
        const std::size_t last_slash {file_path.find_last_of("/")};
        SM_ASSERT(last_slash != std::string::npos, "Could not find slash");

        return file_path.substr(last_slash + 1);
    }

    std::string Utils::get_directory_name(const std::string& file_path) {  // FIXME ?????
        SM_ASSERT(file_path.size() < 512, "File path too large");

        char file_path_copy[512];
        std::strncpy(file_path_copy, file_path.c_str(), 512 - 1);

        std::vector<std::string> tokens;

        char* token {std::strtok(file_path_copy, "/")};

        while (token != nullptr) {
            tokens.push_back(token);
            token = std::strtok(nullptr, "/");
        }

        SM_ASSERT(tokens.size() >= 2, "Invalid file path name");

        return tokens[tokens.size() - 2];  // It's ok
    }

    std::optional<std::pair<unsigned char*, std::size_t>> Utils::read_file(const std::string& file_path) {
        std::ifstream file {file_path, std::ios::binary};

        if (!file.is_open()) {
            return std::nullopt;
        }

        file.seekg(0, file.end);
        const auto length {file.tellg()};
        file.seekg(0, file.beg);

        char* buffer {new char[length]};
        file.read(buffer, length);

        return std::make_optional(std::make_pair(
            reinterpret_cast<unsigned char*>(buffer),
            static_cast<std::size_t>(length)
        ));
    }
}
