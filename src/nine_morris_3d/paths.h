#pragma once

#include <string>

namespace paths {
    std::string path_for_logs(const char* file) noexcept(false);
    std::string path_for_save_and_options(const char* file) noexcept(false);
    std::string path_for_assets(const char* file_path);
}
