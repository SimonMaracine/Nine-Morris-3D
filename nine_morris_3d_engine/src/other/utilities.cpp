#include "nine_morris_3d_engine/other/utilities.hpp"

#include <fstream>
#include <cstddef>
#include <cassert>

#include <glm/gtc/random.hpp>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    unsigned int utils::random_int(unsigned int end) noexcept {
        assert(end > 0);

        return glm::linearRand(0u, end);
    }

    unsigned int utils::random_int(unsigned int begin, unsigned int end) noexcept {
        assert(end > begin);

        return glm::linearRand(begin, end);
    }

    void utils::center_image(
        float screen_width,
        float screen_height,
        float image_width,
        float image_height,
        float& x,
        float& y,
        float& width,
        float& height
    ) noexcept {
        if (screen_width / screen_height > image_width / image_height) {
            width = screen_width;
            height = screen_width * (image_height / image_width);
            x = 0.0f;
            y = (height - screen_height) / -2.0f;
        } else {
            height = screen_height;
            width = screen_height * (image_width / image_height);
            x = (width - screen_width) / -2.0f;
            y = 0.0f;
        }
    }

    std::string utils::file_name(const std::filesystem::path& file_path) {
        assert(file_path.has_filename());

        return file_path.filename().string();
    }

    std::string utils::file_name(const std::string& file_path) {
        return file_name(std::filesystem::path(file_path));
    }

    std::string utils::read_file_ex(const std::filesystem::path& file_path) {
        std::ifstream stream {file_path, std::ios::binary};

        if (!stream.is_open()) {
            throw ResourceError("Could not open file for reading");
        }

        stream.seekg(0, stream.end);
        const auto length {stream.tellg()};
        stream.seekg(0, stream.beg);

        std::string buffer;
        buffer.resize(static_cast<std::size_t>(length));

        stream.read(buffer.data(), length);

        if (stream.fail()) {
            throw ResourceError("Error reading file");
        }

        return buffer;
    }

    std::string utils::read_file_ex(const std::string& file_path) {
        return read_file_ex(std::filesystem::path(file_path));
    }

    std::string utils::read_file(const std::filesystem::path& file_path) {
        LOG_DEBUG("Reading file `{}`...", file_path.string());

        try {
            return read_file_ex(file_path);
        } catch (const ResourceError& e) {
            SM_THROW_ERROR(ResourceError, "Could not read file `{}`: {}", file_path.string(), e.what());
        }
    }

    std::string utils::read_file(const std::string& file_path) {
        return read_file(std::filesystem::path(file_path));
    }
}
