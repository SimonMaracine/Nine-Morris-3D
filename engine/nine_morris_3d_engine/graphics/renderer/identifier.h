#pragma once

namespace identifier {
    typedef unsigned int Id;

    constexpr Id null = 0;

    void initialize();
    Id generate_id();
}
