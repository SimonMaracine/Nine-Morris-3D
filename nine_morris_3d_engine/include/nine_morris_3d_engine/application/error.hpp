#pragma once

// FIXME use exception classes

// Can only be called after logging is initialized
#define SM_CRITICAL_ERROR(error, ...) \
    LOG_DIST_CRITICAL(__VA_ARGS__); \
    throw error

namespace sm {
    enum class RuntimeError {
        Initialization,
        ResourceLoading,
        Other
    };
}
