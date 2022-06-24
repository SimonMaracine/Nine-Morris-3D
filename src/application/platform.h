#pragma once

/**
 * Include platform.h whenever there is conditional code regarding platform and build mode.
 */
#if defined(__linux__)
    #if defined(__GNUG__)
        #define PLATFORM_GAME_LINUX
    #else
        #error "GCC must be used (for now)"
    #endif
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #if defined(_MSC_VER)
        #define PLATFORM_GAME_WINDOWS
    #else
        #error "MSVC must be used (for now)"
    #endif
#else
    #error "Unsupported platform"
#endif

#ifdef NDEBUG
    #define PLATFORM_GAME_RELEASE
#else
    #define PLATFORM_GAME_DEBUG
#endif
