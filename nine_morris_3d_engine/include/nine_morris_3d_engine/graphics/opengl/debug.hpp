#pragma once

#include <string>
#include <utility>

namespace sm::opengl_debug {
    void initialize() noexcept;
    std::string get_information();
    std::pair<int, int> get_version_number() noexcept;
    const unsigned char* get_opengl_version() noexcept;
    const unsigned char* get_glsl_version() noexcept;
    const unsigned char* get_renderer() noexcept;
    const unsigned char* get_vendor() noexcept;
}
