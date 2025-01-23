#pragma once

#include <string>

namespace sm::openal_debug {
    void check_errors() noexcept;
    std::string get_information();
    const char* get_version() noexcept;
}
