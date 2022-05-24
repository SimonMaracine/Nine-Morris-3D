#pragma once

namespace options {
    static constexpr const char* NORMAL = "normal";
    static constexpr const char* LOW = "low";

    static constexpr const char* FIELD = "field";
    static constexpr const char* AUTUMN = "autumn";

    class OptionsFileError : public std::runtime_error {
    public:
        OptionsFileError(const std::string& message)
            : std::runtime_error(message) {}
        OptionsFileError(const char* message)
            : std::runtime_error(message) {}
    };

    class OptionsFileNotOpenError : public OptionsFileError {
    public:
        OptionsFileNotOpenError(const std::string& message)
            : OptionsFileError(message) {}
        OptionsFileNotOpenError(const char* message)
            : OptionsFileError(message) {}
    };

    struct Options {
        // These are default values
        std::string texture_quality = NORMAL;
        int samples = 2;
        int anisotropic_filtering = 4;
        bool vsync = true;
        bool save_on_exit = true;
        std::string skybox = FIELD;
        bool custom_cursor = true;
        float sensitivity = 1.0f;
    };

    void save_options_to_file(const Options& options) noexcept(false);
    void load_options_from_file(Options& options) noexcept(false);

    void create_options_file() noexcept(false);
    void handle_options_file_not_open_error();
}
