#pragma once

/**
 * Include platform.h whenever there is conditional code regarding platform and build mode.
 */
#if defined(__linux__)
    #if defined(__GNUG__)
        #define NM3D_PLATFORM_LINUX
    #else
        #error "GCC must be used on Linux (for now)"
    #endif
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #if defined(_MSC_VER)
        #define NM3D_PLATFORM_WINDOWS
    #else
        #error "MSVC must be used on Windows (for now)"
    #endif
#else
    #error "Unsupported platform"
#endif

/**
 * There are three build modes:
 *
 * NM3D_PLATFORM_DEBUG
 * NM3D_PLATFORM_RELEASE
 * NM3D_PLATFORM_RELEASE_DISTRIBUTION
 */
#ifdef NM3D_OPTIMIZED  // Defined in CMake
    #ifdef NM3D_DISTRIBUTION  // Defined in CMake
        #define NM3D_PLATFORM_RELEASE_DISTRIBUTION  // Flag for building for distribution
    #else
        #define NM3D_PLATFORM_RELEASE  // Compiled with optimization, but with debug code
    #endif
#else
    #define NM3D_PLATFORM_DEBUG  // Compiled without optimization + additional debug code
#endif
