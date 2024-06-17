#pragma once

#include <cstdint>

// Include platform.hpp whenever there is conditional code regarding platform or build mode
#if defined(__linux__)
    #if defined(__GNUG__)
        #define SM_PLATFORM_LINUX
    #else
        #error "GCC must be used on Linux"
    #endif
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #if defined(_MSC_VER)
        #define SM_PLATFORM_WINDOWS
    #else
        #error "MSVC must be used on Windows"
    #endif
#else
    #error "Unsupported platform"
#endif

// Check for 64-bit architecture
#if SIZE_MAX != 0xFFFFFFFFFFFFFFFF
    #error "Unsupported platform"
#endif

/*
    There are three build modes:

    SM_BUILD_DEBUG
    SM_BUILD_RELEASE
    SM_BUILD_DISTRIBUTION
*/
#ifdef NM3D_RELEASE_MODE  // Defined in CMake
    #ifdef NM3D_DISTRIBUTION_MODE  // Defined in CMake
        #define SM_BUILD_DISTRIBUTION  // Compiled with optimization, without any additional debug code
    #else
        #define SM_BUILD_RELEASE  // Compiled with optimization, but with additional debug code
    #endif
#else
    #define SM_BUILD_DEBUG  // Compiled without optimization and with additional debug code

    #ifdef NM3D_DISTRIBUTION_MODE
        #error "Cannot build in distribution mode while also in debug mode"
    #endif
#endif
