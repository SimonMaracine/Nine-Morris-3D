#pragma once

namespace paths {
    std::string path_for_logs(std::string_view file) noexcept(false);
    std::string path_for_save_and_options(std::string_view file) noexcept(false);
    std::string path_for_assets(std::string_view file_path);
}
