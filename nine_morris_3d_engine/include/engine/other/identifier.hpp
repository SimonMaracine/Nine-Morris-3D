#pragma once

// TODO move to client; don't need in engine

// Generator of unique IDs
namespace sm {
    template<typename T>
    class Iden {
    public:
        constexpr Iden() = default;
        constexpr Iden(const T value)
            : value(value) {}

        constexpr operator T() const { return value; }
        constexpr bool operator==(const Iden& other) const { return value == other.value; }
    private:
        T value {};
    };

    template<typename T>
    class Identifier {
    public:
        Iden<T> generate() {
            counter += static_cast<T>(1);
            return counter;
        }

        static constexpr Iden<T> null {static_cast<T>(0)};
    private:
        Iden<T> counter {null};
    };
}
