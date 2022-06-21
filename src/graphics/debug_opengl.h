#pragma once

#include "application/platform.h"
#include "other/logging.h"

#if defined(NINE_MORRIS_3D_DEBUG)
    #ifdef PRINT_GPU_RAM_ALLOCATED
        #define LOG_ALLOCATION(bytes) \
            if (!_gpu_mem_counter.stopped) { \
                DEB_DEBUG("GPU: Allocated {} bytes ({} MiB)", (bytes), (bytes) / 1049000.0f); \
                _gpu_mem_counter.bytes_allocated += (bytes); \
            }
    #else
        #define LOG_ALLOCATION(bytes) \
            if (!_gpu_mem_counter.stopped) { \
                _gpu_mem_counter.bytes_allocated += (bytes); \
            }
    #endif

    #define LOG_TOTAL_GPU_MEMORY_ALLOCATED() \
        if (!_gpu_mem_counter.stopped) { \
            DEB_INFO("GPU: {} total bytes in use ({} MiB)", _gpu_mem_counter.bytes_allocated, \
                    _gpu_mem_counter.bytes_allocated / 1049000.0f); \
            _gpu_mem_counter.stopped = true; \
        }

    struct GpuMemoryCounter {
        unsigned long long bytes_allocated = 0;
        bool stopped = false;
    };

    extern GpuMemoryCounter _gpu_mem_counter;
#elif defined(NINE_MORRIS_3D_RELEASE)
    #define LOG_ALLOCATION(bytes) (void) 0;
    #define LOG_TOTAL_GPU_MEMORY_ALLOCATED() (void) 0;
#endif

namespace debug_opengl {
    void maybe_initialize_debugging();

    // Get information about OpenGL and dependencies
    std::string get_info();

    std::pair<int, int> get_version_numbers();
    const unsigned char* get_opengl_version();
    const unsigned char* get_glsl_version();
    const unsigned char* get_vendor();
    const unsigned char* get_renderer();
}
