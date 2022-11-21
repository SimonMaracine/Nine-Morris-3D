#pragma once

/*
 * Identifier is okay to be float, since numbers are integers (0.0, 1.0, 2.0 etc.) and the floating-point
 * standard used should be IEEE-754.
 */
namespace identifier {
    class Id {
    public:
        constexpr Id() = default;
        constexpr Id(const float value)
            : value(value) {}
        ~Id() = default;

        operator float() const { return value; }
        Id& operator+=(const Id& other) { value += other.value; return *this; }
        bool operator==(const Id& other) { return value == other.value; }
    private:
        float value = 0.0f;
    };

    constexpr Id null = 0.0f;

    void initialize();
    Id generate_id();
#if 0
    bool check(Id identifier_generated, Id identifier_unreliable);
#endif
}
