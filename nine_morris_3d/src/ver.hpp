#pragma once

inline constexpr unsigned int VERSION_MAJOR {0};
inline constexpr unsigned int VERSION_MINOR {5};
inline constexpr unsigned int VERSION_PATCH {0};

constexpr unsigned int version_number() {
    return VERSION_MAJOR * 100 + VERSION_MINOR * 10 + VERSION_PATCH * 1;
}
