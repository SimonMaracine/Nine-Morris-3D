#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <initializer_list>
#include <cstddef>

namespace sm {
    namespace internal {
        class ShaderLibrary {
        public:
            explicit ShaderLibrary(std::initializer_list<std::string> include_directories);

            // Only the source is needed; shaders are included relative to (not including) the include directories
            std::string load_shader(const std::string& source) const;

            void load_shaders_from_include_directories(std::initializer_list<std::string> include_directories);
        private:
            std::string match_and_include(std::string&& string) const;
            static std::string get_include_argument(const std::string& string);
            static std::size_t get_line(std::string::const_iterator begin, std::string::const_iterator end);

            std::unordered_map<std::string, std::string> include_shader_sources;
        };
    }
}
