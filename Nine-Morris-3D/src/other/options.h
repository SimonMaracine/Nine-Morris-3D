#pragma once

#include <engine/engine_other.h>

namespace options {
    class OptionsFileError : public std::runtime_error {
    public:
        explicit OptionsFileError(const std::string& message)
            : std::runtime_error(message) {}
        explicit OptionsFileError(const char* message)
            : std::runtime_error(message) {}
    };

    class OptionsFileNotOpenError : public OptionsFileError {
    public:
        explicit OptionsFileNotOpenError(const std::string& message)
            : OptionsFileError(message) {}
        explicit OptionsFileNotOpenError(const char* message)
            : OptionsFileError(message) {}
    };

    template<typename Opt>
    using _Validate = std::function<std::pair<bool, std::string>(const Opt&)>;

    template<typename Opt>
    void save_options_to_file(const Opt& options, std::string_view options_file_name) noexcept(false) {
        const std::string file_path = file_system::path_for_saved_data(options_file_name);

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file `" + file_path + "` for writing"
            );
        }

        try {
            cereal::BinaryOutputArchive output {file};
            output(options);
        } catch (const std::exception& e) {  // Just to be sure...
            throw OptionsFileError(e.what());
        }

        DEB_INFO("Saved options to file `{}`", file_path);
    }

    template<typename Opt>
    void load_options_from_file(Opt& options, std::string_view options_file_name,
            const _Validate<Opt>& validate) noexcept(false) {
        const std::string file_path = file_system::path_for_saved_data(options_file_name);

        std::ifstream file {file_path, std::ios::binary};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file `" + file_path + "` for reading"
            );
        }

        Opt temporary;

        try {
            cereal::BinaryInputArchive input {file};
            input(temporary);
        } catch (const cereal::Exception& e) {
            throw OptionsFileError(e.what());
        } catch (const std::exception& e) {
            throw OptionsFileError(e.what());
        }

        auto [result, message] = validate(temporary);

        if (!result) {
            throw OptionsFileError(message);
        }

        options = temporary;

        DEB_INFO("Loaded options from file `{}`", file_path);
    }

    template<typename Opt>
    void create_options_file(std::string_view options_file_name) noexcept(false) {
        const std::string file_path = file_system::path_for_saved_data(options_file_name);

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file `" + file_path + "` for writing"
            );
        }

        Opt options;

        try {
            cereal::BinaryOutputArchive output {file};
            output(options);
        } catch (const std::exception& e) {  // Just to be sure...
            throw OptionsFileError(e.what());
        }

        DEB_INFO("Created options file `{}`", file_path);
    }

    template<typename Opt>
    void handle_options_file_not_open_error(std::string_view options_file_name) {
#ifdef NM3D_PLATFORM_RELEASE
        const bool exists = file_system::directory_exists(
            file_system::cut_slash(file_system::path_for_saved_data())
        );

        if (!exists) {
            REL_WARNING("User data directory missing; creating it...");

            const bool success = file_system::create_directory(file_system::path_for_saved_data());

            if (!success) {
                REL_ERROR("Could not create user data directory");
                return;
            }
        }
#endif

        try {
            create_options_file<Opt>(options_file_name);
        } catch (const OptionsFileNotOpenError& e) {
            REL_ERROR("Could not create options file: {}", e.what());
        } catch (const OptionsFileError& e) {
            REL_ERROR("Could not create options file: {}", e.what());
        }
    }
}
