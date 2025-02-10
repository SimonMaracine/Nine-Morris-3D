#pragma once

inline constexpr unsigned int VERSION_MAJOR {0};
inline constexpr unsigned int VERSION_MINOR {3};
inline constexpr unsigned int VERSION_PATCH {0};

constexpr unsigned int version_number(unsigned int major, unsigned int minor, unsigned int patch) {
    return major * 100 + minor * 10 + patch * 1;
}

constexpr unsigned int version_number() {
    return version_number(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}
