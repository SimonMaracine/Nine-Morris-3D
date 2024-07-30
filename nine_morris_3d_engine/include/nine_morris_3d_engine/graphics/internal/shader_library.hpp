#pragma once

#include <string>
#include <unordered_map>
#include <initializer_list>
#include <utility>
#include <cstddef>

namespace sm::internal {
    class ShaderLibrary {
    public:
        explicit ShaderLibrary(std::initializer_list<std::string> include_directories);

        using Define = std::pair<std::string, std::string>;

        // Include shaders relative to (not including) the include directories
        std::string load_shader(const std::string& source) const;

        // Replace defines with their respective values
        std::string load_shader(const std::string& source, std::initializer_list<Define> defines) const;

        void load_shaders_from_include_directories(std::initializer_list<std::string> include_directories);
    private:
        std::string match_and_include(std::string&& string) const;
        static std::string get_include_argument(const std::string& string);
        static std::size_t get_line(std::string::const_iterator begin, std::string::const_iterator end);

        std::unordered_map<std::string, std::string> m_include_shader_sources;
    };
}
