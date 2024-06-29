#pragma once

#include "nine_morris_3d_engine/audio/openal/listener.hpp"

struct ALCdevice;
struct ALCcontext;

namespace sm {
    class Ctx;

    class OpenAlContext {
    private:
        explicit OpenAlContext(bool create = true);
    public:
        ~OpenAlContext();

        OpenAlContext(const OpenAlContext&) = delete;
        OpenAlContext& operator=(const OpenAlContext&) = delete;
        OpenAlContext(OpenAlContext&&) = delete;
        OpenAlContext& operator=(OpenAlContext&&) = delete;

        AlListener& get_listener() { return listener; }
    private:
        bool create {};
        AlListener listener;

        ALCdevice* device {nullptr};
        ALCcontext* context {nullptr};

        friend class Ctx;
    };
}
