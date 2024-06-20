#include "engine/graphics/shader_library.hpp"

#include <filesystem>
#include <regex>
#include <iostream>

namespace sm {
    std::string ShaderLibrary::load_shader(const std::string& source) {

    }

    void ShaderLibrary::load_shaders_from_include_directories(std::initializer_list<std::string> include_directories) {
        for (const auto& include_directory : include_directories) {
            for (std::filesystem::directory_entry entry : std::filesystem::recursive_directory_iterator(include_directory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".glsl") {
                    std::cout << entry << " is a shader source\n";
                }
            }
        }
    }
}

// ^\s*#include\s*"\w+"\s*$
