#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <initializer_list>
#include <utility>

namespace sm::internal {
    // Used to load shader code with includes
    class ShaderLibrary {
    public:
        explicit ShaderLibrary(std::initializer_list<std::filesystem::path> include_directories);

        // Preprocessor definition-like
        using Definition = std::pair<std::string, std::string>;

        // Include shaders relative to (not including) the include directories
        std::string load_shader(const std::string& source) const;

        // Also replace defines with their respective values
        std::string load_shader(const std::string& source, std::initializer_list<Definition> defines) const;
    private:
        void load_shaders_from_include_directories(std::initializer_list<std::filesystem::path> include_directories);
        std::string match_and_include(std::string&& string, std::size_t count) const;
        static std::string get_include_argument(const std::string& string);
        static std::size_t get_line(std::string::const_iterator begin, std::string::const_iterator end);

        std::unordered_map<std::string, std::string> m_include_shader_sources;
    };
}
