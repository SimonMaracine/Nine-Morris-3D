#pragma once

#include "nine_morris_3d_engine/nine_morris_3d_engine.h"

namespace options {
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

    static std::string get_file_path(std::string_view options_file_name) noexcept(false) {
        std::string file_path;

        try {
            file_path = paths::path_for_saved_data(options_file_name);
        } catch (const user_data::UserNameError& e) {
            throw OptionsFileError(e.what());
        }

        return file_path;
    }

    template<typename OptionsType>
    using _Validate = std::function<std::pair<bool, std::string>(const OptionsType&)>;

    template<typename OptionsType>
    void save_options_to_file(const OptionsType& options, std::string_view options_file_name) noexcept(false) {
        const std::string file_path = get_file_path(options_file_name);

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "' for writing"
            );
        }

        try {
            cereal::BinaryOutputArchive output {file};
            output(options);
        } catch (const std::exception& e) {  // Just to be sure...
            throw OptionsFileError(e.what());
        }

        DEB_INFO("Saved options to file '{}'", file_path);
    }

    template<typename OptionsType>
    void load_options_from_file(OptionsType& options, std::string_view options_file_name,
            const _Validate<OptionsType>& validate) noexcept(false) {
        const std::string file_path = get_file_path(options_file_name);

        std::ifstream file {file_path, std::ios::binary};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "'"
            );
        }

        OptionsType temporary;

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

        DEB_INFO("Loaded options from file '{}'", file_path);
    }

    template<typename OptionsType>
    void create_options_file(std::string_view options_file_name) noexcept(false) {
        const std::string file_path = get_file_path(options_file_name);

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw OptionsFileNotOpenError(
                "Could not open options file '" + file_path + "' for writing"
            );
        }

        OptionsType options;

        try {
            cereal::BinaryOutputArchive output {file};
            output(options);
        } catch (const std::exception& e) {  // Just to be sure...
            throw OptionsFileError(e.what());
        }

        DEB_INFO("Created options file '{}'", file_path);
    }

    template<typename OptionsType>
    void handle_options_file_not_open_error(std::string_view options_file_name, std::string_view app_name) {
        bool user_data_directory;

        try {
            user_data_directory = user_data::user_data_directory_exists(app_name);
        } catch (const user_data::UserNameError& e) {
            REL_ERROR("{}", e.what());
            return;
        }

        if (!user_data_directory) {
            REL_INFO("User data folder missing; creating one...");

            try {
                const bool success = user_data::create_user_data_directory(app_name);
                if (success) {
                    try {
                        create_options_file<OptionsType>(options_file_name);
                    } catch (const OptionsFileNotOpenError& e) {
                        REL_ERROR("{}", e.what());
                        return;
                    } catch (const OptionsFileError& e) {
                        REL_ERROR("{}", e.what());
                        return;
                    }
                } else {
                    REL_ERROR("Could not create user data directory");
                    return;
                }
            } catch (const user_data::UserNameError& e) {
                REL_ERROR("{}", e.what());
                return;
            }
        } else {
            try {
                create_options_file<OptionsType>(options_file_name);
            } catch (const OptionsFileNotOpenError& e) {
                REL_ERROR("{}", e.what());
                return;
            } catch (const OptionsFileError& e) {
                REL_ERROR("{}", e.what());
                return;
            }
        }
    }
}
