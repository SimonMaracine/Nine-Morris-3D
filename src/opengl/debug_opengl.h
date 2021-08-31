#pragma once

#include <string>
#include <utility>

#define LOG_ALLOCATION(bytes) \
    if (!stop_counting_bytes_allocated_gpu) { \
        SPDLOG_DEBUG("GPU: Allocated {} bytes", (bytes)); \
        approximately_bytes_allocated_gpu += (bytes); \
        SPDLOG_DEBUG("GPU: Now there are {} bytes allocated", approximately_bytes_allocated_gpu); \
    }

#define STOP_ALLOCATION_LOG \
    stop_counting_bytes_allocated_gpu = true; \
    SPDLOG_DEBUG("GPU: Stop counting memory allocated");

extern unsigned long long approximately_bytes_allocated_gpu;
extern bool stop_counting_bytes_allocated_gpu;

namespace debug_opengl {
    void maybe_init_debugging();
    const std::string get_info();
    const std::pair<int, int> get_version();
}
