#include "engine/other/utilities.hpp"

#include <fstream>
#include <cstddef>

namespace sm {
    std::optional<std::string> utils::read_file(const std::string& file_path) {  // FIXME error handling
        std::ifstream stream {file_path, std::ios::binary};

        if (!stream.is_open()) {
            return std::nullopt;
        }

        stream.seekg(0, stream.end);
        const auto length {stream.tellg()};
        stream.seekg(0, stream.beg);

        char* buffer {new char[length]};
        stream.read(buffer, length);

        const std::string contents {buffer, static_cast<std::size_t>(length)};

        delete[] buffer;

        return std::make_optional(contents);
    }
}
