#pragma once

#include "application/application.h"
#include "nine_morris_3d/options.h"

constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

class NineMorris3D : public Application {
public:
    NineMorris3D();

    static void set_app_pointer(NineMorris3D* instance);

    options::Options options;
    unsigned int arrow_cursor = 0;
    unsigned int cross_cursor = 0;
};

extern NineMorris3D* app;
