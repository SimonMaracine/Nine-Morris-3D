#pragma once

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

// Used to differentiate between testing and distribution builds
#ifdef NM3D_DISTRIBUTION_MODE  // Defined in CMake
    #define SM_BUILD_DISTRIBUTION
#endif
