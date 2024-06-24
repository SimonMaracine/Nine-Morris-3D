#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <initializer_list>

namespace sm {
    class Application;
    class Ctx;

    class ShaderLibrary {
    public:
        // Only the source is needed; shaders are included relative to (not including) the include directories
        std::string load_shader(const std::string& source) const;
    private:
        explicit ShaderLibrary(std::initializer_list<std::string> include_directories);

        void load_shaders_from_include_directories(std::initializer_list<std::string> include_directories);
        std::string match_and_include(std::size_t line, std::string&& line_string) const;
        static std::string get_include_argument(const std::string& string);

        std::unordered_map<std::string, std::string> include_shader_sources;

        friend class Application;
        friend class Ctx;
    };
}
