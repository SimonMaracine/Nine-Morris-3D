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
        std::vector<std::pair<std::size_t, std::string>> lines;
        std::size_t last_position {};  // TODO C++20
        std::size_t position {};
        std::size_t line {1};

        for (const char character : source) {
            if (character == '\n' || character == '\r' || position == source.size() - 1) {
                lines.push_back(std::make_pair(line, std::string(source, last_position, position - last_position + 1)));
                last_position = position + 1;
                line++;
            }

            position++;
        }

        std::string result;

        for (auto&& line : lines) {
            result += match_and_include(line.first, std::move(line.second));
        }

        return result;
    }

    ShaderLibrary::ShaderLibrary(std::initializer_list<std::string> include_directories) {
        load_shaders_from_include_directories(include_directories);
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

    std::string ShaderLibrary::match_and_include(std::size_t line, std::string&& line_string) const {
        const std::regex pattern {R"(^\s*#include\s*"[\w\/\.]+"\s*$)"};
        std::smatch results;

        if (!std::regex_match(line_string, results, pattern)) {
            return line_string;
        }

        if (results.size() != 1) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Invalid #include directive");
        }

        const auto& match {*results.cbegin()};
        const auto argument {get_include_argument(match.str())};
        const auto iter {include_shader_sources.find(argument)};

        if (iter == include_shader_sources.cend()) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Cannot include `{}`; file not found", argument);
        }

        line_string.clear();
        line_string += "#line 1\n";
        line_string += load_shader(iter->second) + '\n';
        line_string += "#line " + std::to_string(line + 1) + '\n';

        return line_string;
    }

    std::string ShaderLibrary::get_include_argument(const std::string& string) {
        const auto first_quote {string.find_first_of("\"")};
        const auto last_quote {string.find_last_of("\"")};

        return string.substr(first_quote + 1, last_quote - first_quote - 1);
    }
}
