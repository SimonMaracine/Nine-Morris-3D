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
    #error "No support for 32-bit"
#endif

// Used to differentiate between testing and distribution builds
#ifdef NM3D_DISTRIBUTION_MODE  // Defined in CMake
    #define SM_BUILD_DISTRIBUTION  // Compiled with optimization, without any additional debug code
#endif

// Else compiled with or without optimization and with additional debug code
