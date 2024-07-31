#include "nine_morris_3d_engine/graphics/internal/shader_library.hpp"

#include <filesystem>
#include <regex>
#include <iterator>
#include <vector>
#include <sstream>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm::internal {
    ShaderLibrary::ShaderLibrary(std::initializer_list<std::string> include_directories) {
        load_shaders_from_include_directories(include_directories);
    }

    std::string ShaderLibrary::load_shader(const std::string& source) const {
#if defined(SM_PLATFORM_LINUX)
        return match_and_include(std::string(source), 0);
#elif defined(SM_PLATFORM_WINDOWS)
        std::string result;

        std::stringstream stream {source};
        std::string line;
        std::size_t count {0};

        while (std::getline(stream, line)) {
            if (line.back() == '\r') {  // Stupid Windows :P
                line.pop_back();
            }

            result += match_and_include(std::move(line), ++count) + '\n';
        }

        return result;
#endif
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
            std::error_code ec;
            auto iter {std::filesystem::recursive_directory_iterator(include_directory, ec)};

            if (ec) {
                SM_THROW_ERROR(ResourceError, "Could not iterate over include directory `{}`: {}", include_directory, ec.message());
            }

            for (const std::filesystem::directory_entry& entry : iter) {
                if (entry.is_regular_file() && entry.path().extension() == ".glsl") {
                    const auto file_path {entry.path()};

                    m_include_shader_sources[file_path.filename().string()] = utils::read_file(file_path);
                    LOG_DEBUG("Loaded shader `{}` as `{}`", file_path.string(), file_path.filename().string());
                }
            }
        }
    }

    std::string ShaderLibrary::match_and_include(std::string&& string, [[maybe_unused]] std::size_t count) const {
        const std::regex pattern (
            R"(^[ \t]*#include[ \t]*"[\w\-\/\.]+"[ \t]*$)",
#if defined(SM_PLATFORM_LINUX)
            std::regex::ECMAScript | std::regex::multiline
#elif defined(SM_PLATFORM_WINDOWS)
            std::regex::ECMAScript  // Stupid Windows :P
#endif
        );

        std::smatch results;

        std::sregex_iterator begin_regex {string.cbegin(), string.cend(), pattern};
        std::sregex_iterator end_regex;

        std::vector<std::tuple<std::string::const_iterator, std::string::const_iterator, std::size_t, std::string>> matches;

        for (auto iter {begin_regex}; iter != end_regex; iter++) {
            const auto& match {*iter};
            const auto& sub_match {*match.cbegin()};

            const auto argument {get_include_argument(match.str())};
            const auto iter_arg {m_include_shader_sources.find(utils::file_name(argument))};

            if (iter_arg == m_include_shader_sources.cend()) {
                SM_THROW_ERROR(ResourceError, "Cannot include `{}`; file not found", argument);
            }

#if defined(SM_PLATFORM_LINUX)
            const std::size_t line {get_line(string.cbegin(), sub_match.first)};
#elif defined(SM_PLATFORM_WINDOWS)
            const std::size_t line {count};
#endif

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
