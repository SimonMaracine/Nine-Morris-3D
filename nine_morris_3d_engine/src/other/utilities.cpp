#include "engine/other/utilities.hpp"

#include <fstream>
#include <cstddef>

#include "engine/application_base/logging.hpp"
#include "engine/application_base/error.hpp"

namespace sm {
    std::string utils::read_file_ex(const std::string& file_path) {
        std::ifstream stream {file_path, std::ios::binary};

        if (!stream.is_open()) {
            throw FileReadError("Could not open file for reading");
        }

        stream.seekg(0, stream.end);
        const auto length {stream.tellg()};
        stream.seekg(0, stream.beg);

        std::string buffer;
        buffer.resize(static_cast<std::size_t>(length));

        stream.read(buffer.data(), length);

        if (stream.fail()) {
            throw FileReadError("Error reading file");
        }

        return buffer;
    }

    std::string utils::read_file(const std::string& file_path) {
        LOG_DEBUG("Reading file `{}`...", file_path);

        try {
            return read_file_ex(file_path);
        } catch (const utils::FileReadError& e) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Could not read file `{}`: {}", file_path, e.what());
        }
    }
}
