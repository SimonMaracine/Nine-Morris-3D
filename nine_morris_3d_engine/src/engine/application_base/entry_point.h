#pragma once

#include "engine/application_base/platform.h"

/**
 * Define a pseudo main function with this declaration:
 *
 * void application_main();
 *
 * And include this engine_entry_point.h header file once into your main compilation unit.
 */

void application_main();

#if defined(NM3D_PLATFORM_LINUX)
    int main(int, char**) {
        application_main();
    }
#elif defined(NM3D_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>

    #if defined(NM3D_PLATFORM_DISTRIBUTION)
        INT WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT) {
            application_main();
        }
    #elif defined(NM3D_PLATFORM_DEBUG)
        int main(int, char**) {
            application_main();
        }
    #endif
#endif
