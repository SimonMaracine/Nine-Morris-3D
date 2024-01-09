#pragma once

/*
    Generator of unique IDs.

    Id is okay to be even float, since numbers are integers (0.0, 1.0, 2.0 etc.) and the
    floating-point standard used should be IEEE-754.
*/
namespace sm {
    template<typename T>
    class Id {
    public:
        constexpr Id() = default;
        constexpr Id(const T value)
            : value(value) {}

        constexpr operator T() const { return value; }
        constexpr bool operator==(const Id& other) const { return value == other.value; }
    private:
        T value {};
    };

    template<typename T>
    class Identifier {
    public:
        Id<T> generate() {
            counter += static_cast<T>(1);
            return counter;
        }

        static constexpr Id<T> null {static_cast<T>(0)};
    private:
        Id<T> counter {null};
    };
}
