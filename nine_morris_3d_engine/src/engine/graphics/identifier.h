#pragma once

namespace sm {
    /**
     * Generator of unique IDs.
     *
     * Id is okay to be float, since numbers are integers (0.0, 1.0, 2.0 etc.) and the
     * floating-point standard used should be IEEE-754.
     */
    class Identifier final {
    public:
        Identifier() = default;
        ~Identifier() = default;

        Identifier(const Identifier&) = delete;
        Identifier& operator=(const Identifier&) = delete;
        Identifier(Identifier&&) = delete;
        Identifier& operator=(Identifier&&) = delete;

        class Id {
        public:
            constexpr Id() = default;
            constexpr Id(const float value)
                : value(value) {}

            constexpr operator float() const { return value; }
            Id& operator+=(const Id& other) { value += other.value; return *this; }
            constexpr bool operator==(const Id& other) const { return value == other.value; }
        private:
            float value = 0.0f;
        };

        Id generate();

#if 0
        static bool check(Id identifier_generated, Id identifier_unreliable);
#endif

        static Id null;  // Should have been static constexpr, but compiler is too dumb
    private:
        Id id = 0.0f;
    };
}
