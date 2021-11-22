#pragma once

#include <string>
#include <utility>

#ifndef NDEBUG
    #ifdef PRINT_GPU_RAM_ALLOCATED
        #define LOG_ALLOCATION(bytes) \
            if (!stop_counting_bytes_allocated_gpu) { \
                SPDLOG_DEBUG("GPU: Allocated {} bytes ({} MiB)", (bytes), (bytes) / 1049000.0f); \
                bytes_allocated_gpu += (bytes); \
            }
    #else
        #define LOG_ALLOCATION(bytes) \
            if (!stop_counting_bytes_allocated_gpu) { \
                bytes_allocated_gpu += (bytes); \
            }
    #endif

    #define STOP_ALLOCATION_LOG() \
        if (!stop_counting_bytes_allocated_gpu) { \
            SPDLOG_INFO("GPU: Stop counting memory allocated"); \
            SPDLOG_INFO("GPU: {} total bytes allocated ({} MiB)", bytes_allocated_gpu, bytes_allocated_gpu / 1049000.0f); \
        } \
        stop_counting_bytes_allocated_gpu = true;

extern unsigned long long bytes_allocated_gpu;
extern bool stop_counting_bytes_allocated_gpu;
#else
    #define LOG_ALLOCATION(bytes) ((void) 0)
    #define STOP_ALLOCATION_LOG() ((void) 0)
#endif

namespace debug_opengl {
    void maybe_initialize_debugging();
    const std::string get_info();
    const std::pair<int, int> get_version();
}
