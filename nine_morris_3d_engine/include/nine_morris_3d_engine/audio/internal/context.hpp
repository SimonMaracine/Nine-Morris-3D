#pragma once

// https://www.openal.org/documentation/openal-1.1-specification.pdf

struct ALCdevice;
struct ALCcontext;

namespace sm::internal {
    class OpenAlContext {
    public:
        explicit OpenAlContext(bool create = true);
        ~OpenAlContext() noexcept;

        OpenAlContext(const OpenAlContext&) = delete;
        OpenAlContext& operator=(const OpenAlContext&) = delete;
        OpenAlContext(OpenAlContext&&) = delete;
        OpenAlContext& operator=(OpenAlContext&&) = delete;
    private:
        bool create {};

        ALCdevice* device {};
        ALCcontext* context {};
    };
}
