#include "nine_morris_3d_engine/graphics/internal/shader_library.hpp"

#include <filesystem>
#include <regex>
#include <iterator>
#include <vector>
#include <utility>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm::internal {
    ShaderLibrary::ShaderLibrary(std::initializer_list<std::string> include_directories) {
        load_shaders_from_include_directories(include_directories);
    }

    std::string ShaderLibrary::load_shader(const std::string& source) const {
        return match_and_include(std::string(source));
    }

    std::string ShaderLibrary::load_shader(const std::string& source, std::initializer_list<Define> defines) const {
        std::string result {source};

        for (const auto& define : defines) {
            std::size_t index {};

            while (true) {
                const auto position {result.find(define.first, index)};

                if (position == std::string::npos) {
                    break;
                }

                result.replace(position, define.first.size(), define.second);

                index = position + define.second.size();
            }
        }

        return result;
    }

    void ShaderLibrary::load_shaders_from_include_directories(std::initializer_list<std::string> include_directories) {
        for (const auto& include_directory : include_directories) {
            for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(include_directory)) {  // FIXME throws exception
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
        const std::regex pattern (R"(^[ \t]*#include[ \t]*"[\w\-\/\.]+"[ \t]*$)", std::regex::ECMAScript | std::regex::multiline);
        std::smatch results;

        std::sregex_iterator begin_regex {string.cbegin(), string.cend(), pattern};
        std::sregex_iterator end_regex;

        std::vector<std::tuple<std::string::const_iterator, std::string::const_iterator, std::size_t, std::string>> matches;

        for (auto iter {begin_regex}; iter != end_regex; iter++) {
            const auto& match {*iter};
            const auto& sub_match {*match.cbegin()};

            const auto argument {get_include_argument(match.str())};
            const auto iter_arg {include_shader_sources.find(argument)};

            if (iter_arg == include_shader_sources.cend()) {
                SM_THROW_ERROR(ResourceError, "Cannot include `{}`; file not found", argument);
            }

            const std::size_t line {get_line(string.cbegin(), sub_match.first)};

            matches.push_back(std::make_tuple(sub_match.first, sub_match.second, line, iter_arg->second));
        }

        if (matches.empty()) {
            return string;
        }

        std::string result;
        std::string::const_iterator begin {string.cbegin()};

        for (auto iter {matches.begin()}; iter != matches.end(); iter++) {
            std::string::const_iterator end;

            if (std::next(iter) != matches.end()) {
                end = std::get<0>(*std::next(iter));
            } else {
                end = string.cend();
            }

            result += std::string(begin, std::get<0>(*iter));
            result += "#line 1\n" + load_shader(std::get<3>(*iter)) + "\n#line " + std::to_string(std::get<2>(*iter) + 1);

            begin = std::get<1>(*iter);
        }

        result += std::string(std::get<1>(*std::prev(matches.cend())), string.cend());

        return result;
    }

    std::string ShaderLibrary::get_include_argument(const std::string& string) {
        const auto first_quote {string.find_first_of("\"")};
        const auto last_quote {string.find_last_of("\"")};

        return string.substr(first_quote + 1, last_quote - first_quote - 1);
    }

    std::size_t ShaderLibrary::get_line(std::string::const_iterator begin, std::string::const_iterator end) {
        return std::count(begin, end, '\n') + 1;
    }
}
