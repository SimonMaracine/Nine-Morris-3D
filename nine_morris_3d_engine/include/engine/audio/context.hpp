#pragma once

#include "engine/audio/openal/listener.hpp"

struct ALCdevice;
struct ALCcontext;

namespace sm {
    class OpenAlContext {
    public:
        OpenAlContext();
        ~OpenAlContext();

        OpenAlContext(const OpenAlContext&) = delete;
        OpenAlContext& operator=(const OpenAlContext&) = delete;
        OpenAlContext(OpenAlContext&&) = delete;
        OpenAlContext& operator=(OpenAlContext&&) = delete;

        AlListener& get_listener() { return listener; }
    private:
        AlListener listener;

        ALCdevice* device {nullptr};
        ALCcontext* context {nullptr};
    };
}
