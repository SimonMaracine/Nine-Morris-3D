#include "nine_morris_3d_engine/application/application_builder.h"

ApplicationBuilder& ApplicationBuilder::display(int width, int height, std::string_view title) {
    this->width = width;
    this->height = height;
    this->title = title;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::display_flags(bool fullscreen, bool native_resolution, bool resizable) {
    this->fullscreen = fullscreen;
    this->native_resolution = native_resolution;
    this->resizable = resizable;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::display_min_resolution(int min_width, int min_height) {
    this->min_width = min_width;
    this->min_height = min_height;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::file_names(std::string_view application_name,
        std::string_view info_file_name) {
    this->application_name = application_name;
    this->info_file_name = info_file_name;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::version(unsigned int major, unsigned int minor, unsigned int patch) {
    this->major = major;
    this->minor = minor;
    this->patch = patch;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::authors(const std::vector<std::string>& author_list) {
    this->author_list = author_list;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::encrypt_key(std::string_view encryption_key) {
    this->encryption_key = encryption_key;

    return *this;
}

ApplicationBuilder& ApplicationBuilder::with_renderer(Renderer renderer) {
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

ApplicationBuilder& ApplicationBuilder::with_audio() {
    audio = true;

    return *this;
}
