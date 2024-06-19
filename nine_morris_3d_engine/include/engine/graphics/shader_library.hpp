#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <initializer_list>

namespace sm {
    class Application;

    class ShaderLibrary {
    public:
        std::string load_shader(const std::string& source);
    private:
        void load_shaders_from_include_directories(std::initializer_list<std::string> include_directories);

        std::vector<std::string> include_shader_sources;

        friend class Application;
    };

    struct ProcessError : public std::runtime_error {
        explicit ProcessError(const std::string& message)
            : runtime_error(message) {}
        explicit ProcessError(const char* message)
            : runtime_error(message) {}
    };
}
