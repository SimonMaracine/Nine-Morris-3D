#include "nine_morris_3d_engine/application/application_builder.h"

ApplicationBuilder& ApplicationBuilder::display_config(int width, int height, std::string_view title,
        bool fullscreen, bool native_resolution, bool resizable, int min_width, int min_height) {
    this->width = width;
    this->height = height;
    this->title = title;
    this->fullscreen = fullscreen;
    this->native_resolution = native_resolution;
    this->resizable = resizable;
    this->min_width = min_width;
    this->min_height = min_height;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::file_names_config(std::string_view application_name,
        std::string_view info_file_name) {
    this->application_name = application_name;
    this->info_file_name = info_file_name;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::version_config(unsigned int major, unsigned int minor, unsigned int patch) {
    this->major = major;
    this->minor = minor;
    this->patch = patch;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::authors_config(const std::vector<std::string>& authors) {
    this->authors = authors;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::encrypt_key_config(std::string_view encrypt_key) {
    this->encrypt_key = encrypt_key;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::with(Renderer renderer) {
    switch (renderer) {
        case Renderer::R3D:
            renderer_3d = true;
            break;
        case Renderer::R2D:
            renderer_2d = true;
            break;
        case Renderer::RImGui:
            renderer_imgui = true;
            break;
    }

    return *this;
}
