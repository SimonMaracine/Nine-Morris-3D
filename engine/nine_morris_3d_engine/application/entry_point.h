#pragma once

#include "nine_morris_3d_engine/application/platform.h"

/**
 * Define a pseudo main function with this declaration:
 *
 * void application_main();
 */
#if defined(NM3D_PLATFORM_LINUX)
    #define DEFINE_MAIN_FUNCTION() \
        int main(int argc, char** argv) { \
            application_main(); \
        }
#elif defined(NM3D_PLATFORM_WINDOWS)  // TODO test on Windows
    #define DEFINE_MAIN_FUNCTION() \
        #include <Windows.h> \
        int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) { \
            application_main(); \
        }
#endif
