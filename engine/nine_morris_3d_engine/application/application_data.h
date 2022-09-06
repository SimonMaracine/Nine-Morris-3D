#pragma once

#include "nine_morris_3d_engine/application/events.h"

class Application;

struct ApplicationData {
    int width, height;
    std::string title;
    int min_width;
    int min_height;
    std::string application_name;
    std::string info_file_name;
    unsigned int version_major;
    unsigned int version_minor;
    unsigned int version_patch;
    std::vector<std::string> authors;

    Application* app;
};
