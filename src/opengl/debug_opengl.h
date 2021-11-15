#pragma once

#include <string>
#include <utility>

#ifndef NDEBUG
    #ifdef PRINT_GPU_RAM_ALLOCATED
        #define LOG_ALLOCATION(bytes) \
            if (!stop_counting_bytes_allocated_gpu) { \
                SPDLOG_DEBUG("GPU: Allocated {} bytes", (bytes)); \
                approximately_bytes_allocated_gpu += (bytes); \
            }
    #else
        #define LOG_ALLOCATION(bytes) \
            if (!stop_counting_bytes_allocated_gpu) { \
                approximately_bytes_allocated_gpu += (bytes); \
            }
    #endif

    #define STOP_ALLOCATION_LOG() \
        stop_counting_bytes_allocated_gpu = true; \
        SPDLOG_DEBUG("GPU: Stop counting memory allocated"); \
        SPDLOG_DEBUG("GPU: {} total bytes allocated", approximately_bytes_allocated_gpu);

extern unsigned long long approximately_bytes_allocated_gpu;
extern bool stop_counting_bytes_allocated_gpu;
#else
    #define LOG_ALLOCATION(bytes) ((void) 0)
    #define STOP_ALLOCATION_LOG() ((void) 0)
#endif

namespace debug_opengl {
    void maybe_init_debugging();
    const std::string get_info();
    const std::pair<int, int> get_version();
}
