#pragma once

/*
    Generator of unique IDs.

    Id is okay to be float, since numbers are integers (0.0, 1.0, 2.0 etc.) and the
    floating-point standard used should be IEEE-754.
*/
namespace sm {
    class Id {
    public:
        constexpr Id() = default;
        constexpr Id(const float value)
            : value(value) {}

        constexpr operator float() const { return value; }
        Id& operator+=(const Id& other) { value += other.value; return *this; }
        constexpr bool operator==(const Id& other) const { return value == other.value; }
    private:
        float value {0.0f};
    };

    class Identifier {
    public:
        Identifier() = default;
        ~Identifier() = default;

        Identifier(const Identifier&) = delete;
        Identifier& operator=(const Identifier&) = delete;
        Identifier(Identifier&&) = delete;
        Identifier& operator=(Identifier&&) = delete;

        Id generate();

        static constexpr Id null {Id()};
    private:
        Id id {0.0f};
    };
}
