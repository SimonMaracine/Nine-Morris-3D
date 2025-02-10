#pragma once

#include <tuple>

inline constexpr unsigned int VERSION_MAJOR {0};
inline constexpr unsigned int VERSION_MINOR {3};
inline constexpr unsigned int VERSION_PATCH {0};

constexpr unsigned int version_number(unsigned int major, unsigned int minor, unsigned int patch) {
    return major * 10000 + minor * 100 + patch * 1;
}

constexpr std::tuple<unsigned int, unsigned int, unsigned int> version_number(unsigned int version) {
    return std::make_tuple(
        version / 10000 % 100,
        version / 100 % 100,
        version / 1 % 100
    );
}

constexpr unsigned int version_number() {
    return version_number(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}
