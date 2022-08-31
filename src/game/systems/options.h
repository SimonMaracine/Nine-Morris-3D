#pragma once

#include <entt/entt.hpp>

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

void save_options_system(entt::registry& registry) noexcept(false);
void load_options_system(entt::registry& registry) noexcept(false);

void create_options_file() noexcept(false);
void handle_options_file_not_open_error();
