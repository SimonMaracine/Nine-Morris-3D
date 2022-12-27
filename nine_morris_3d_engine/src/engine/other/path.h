#pragma once

namespace path {
    void initialize_for_applications(std::string_view application_name);
    std::string path_for_logs(std::string_view file) noexcept(false);
    std::string path_for_saved_data(std::string_view file) noexcept(false);
    std::string path_for_assets(std::string_view file_path);
}
