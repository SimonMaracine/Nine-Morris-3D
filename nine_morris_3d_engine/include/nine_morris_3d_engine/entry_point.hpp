#pragma once

#include "nine_morris_3d_engine/application/platform.hpp"

/*
    Define a pseudo main function with this declaration:

    int sm_application_main();

    Include this entry_point.hpp header file once into your main compilation unit.
*/
int sm_application_main();

#if defined(SM_PLATFORM_LINUX)
    int main(int, char**) {
        return sm_application_main();
    }
#elif defined(SM_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>

    #if defined(SM_BUILD_DISTRIBUTION)
        INT WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT) {
            return sm_application_main();
        }
    #else
        int main(int, char**) {
            return sm_application_main();
        }
    #endif
#endif
