#pragma once

#include "engine/application_base/platform.hpp"

/*
    Define a pseudo main function with this declaration:

    void application_main();

    Include this engine_entry_point.h header file once into your main compilation unit.
*/

void application_main();

#if defined(SM_PLATFORM_LINUX)
    int main(int, char**) {
        application_main();
    }
#elif defined(SM_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>

    #if defined(SM_BUILD_DISTRIBUTION)
        INT WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT) {
            application_main();
        }
    #elif defined(SM_BUILD_DEBUG)
        int main(int, char**) {
            application_main();
        }
    #endif
#endif
