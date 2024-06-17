#include "engine/other/utilities.hpp"

#include <vector>
#include <cstring>
#include <fstream>
#include <cassert>

namespace sm {
    std::string Utils::get_file_name(const std::string& file_path) {
        const std::size_t last_slash {file_path.find_last_of("/")};
        assert(last_slash != std::string::npos);

        return file_path.substr(last_slash + 1);
    }

    std::string Utils::get_directory_name(const std::string& file_path) {  // FIXME ?????
        assert(file_path.size() < 512);

        char file_path_copy[512] {};
        std::strncpy(file_path_copy, file_path.c_str(), 512 - 1);

        std::vector<std::string> tokens;

        char* token {std::strtok(file_path_copy, "/")};

        while (token != nullptr) {
            tokens.push_back(token);
            token = std::strtok(nullptr, "/");
        }

        assert(tokens.size() >= 2);

        return tokens[tokens.size() - 2];  // It's ok
    }

    std::optional<std::pair<unsigned char*, std::size_t>> Utils::read_file(const std::string& file_path) {  // FIXME error handling, maybe return unique_ptr
        std::ifstream stream {file_path, std::ios::binary};

        if (!stream.is_open()) {
            return std::nullopt;
        }

        stream.seekg(0, stream.end);
        const auto length {stream.tellg()};
        stream.seekg(0, stream.beg);

        char* buffer {new char[length]};
        stream.read(buffer, length);

        return std::make_optional(std::make_pair(
            reinterpret_cast<unsigned char*>(buffer),
            static_cast<std::size_t>(length)
        ));
    }
}
