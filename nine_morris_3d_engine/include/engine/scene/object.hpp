#pragma once

namespace sm {
    // TODO think about how to do a complete system
    struct Object {
        Object() = default;
        virtual ~Object() = default;

        Object(const Object&) = default;
        Object& operator=(const Object&) = default;
        Object(Object&&) = default;
        Object& operator=(Object&&) = default;
    };
}
