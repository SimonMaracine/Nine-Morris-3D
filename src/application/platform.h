#pragma once

/**
 * Include platform.h whenever there is conditional code regarding platform and build mode.
 */
#if defined(__linux__)
    #if defined(__GNUG__)
        #define NINE_MORRIS_3D_LINUX
    #else
        #error "GCC must be used (for now)"
    #endif
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #if defined(_MSC_VER)
        #define NINE_MORRIS_3D_WINDOWS
    #else
        #error "MSVC must be used (for now)"
    #endif
#else
    #error "Unsupported platform"
#endif

#ifdef NDEBUG
    #define NINE_MORRIS_3D_RELEASE
#else
    #define NINE_MORRIS_3D_DEBUG
#endif
