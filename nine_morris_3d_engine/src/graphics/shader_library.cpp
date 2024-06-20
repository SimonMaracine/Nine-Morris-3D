#include "engine/graphics/shader_library.hpp"

#include <filesystem>
#include <regex>
#include <iterator>
#include <vector>
#include <cstddef>
#include <utility>

#include "engine/application_base/logging.hpp"
#include "engine/application_base/error.hpp"
#include "engine/other/utilities.hpp"

namespace sm {
    std::string ShaderLibrary::load_shader(const std::string& source) const {
        std::vector<std::string> lines;
        std::size_t last_position {};  // TODO C++20
        std::size_t position {};

        for (const char character : source) {
            if (character == '\n' || character == '\r' || position == source.size() - 1) {
                lines.emplace_back(source, last_position, position - last_position + 1);
                last_position = position + 1;
            }

            position++;
        }

        std::string result;

        for (auto&& line : lines) {
            result += match_and_include(std::move(line));
        }

        return result;
    }

    void ShaderLibrary::load_shaders_from_include_directories(std::initializer_list<std::string> include_directories) {
        for (const auto& include_directory : include_directories) {
            for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(include_directory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".glsl") {
                    const auto file_path {entry.path().string()};
                    const auto first_slash {file_path.find_first_of("/")};

                    include_shader_sources[file_path.substr(first_slash + 1)] = utils::read_file(file_path);
                }
            }
        }

        LOG_INFO("Loaded shaders from include directories");
    }

    std::string ShaderLibrary::match_and_include(std::string&& string) const {
        const std::regex pattern {R"(^\s*#include\s*"[\w\/\.]+"\s*$)"};
        std::smatch results;

        if (!std::regex_match(string, results, pattern)) {
            return string;
        }

        for (const auto& match : results) {
            const auto argument {get_include_argument(match.str())};
            const auto iter {include_shader_sources.find(argument)};

            if (iter == include_shader_sources.cend()) {
                SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Cannot include `{}`; file not found", argument);
            }

            string.replace(match.first, match.second, load_shader(iter->second));
        }

        return string;
    }

    std::string ShaderLibrary::get_include_argument(const std::string& string) {
        const auto first_quote {string.find_first_of("\"")};
        const auto last_quote {string.find_last_of("\"")};

        return string.substr(first_quote + 1, last_quote - first_quote - 1);
    }
}
