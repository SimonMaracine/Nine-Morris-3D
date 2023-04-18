#pragma once

#include "engine/audio/openal/listener.h"

struct ALCdevice;
struct ALCcontext;

namespace sm {
    class OpenAlContext final {
    public:
        OpenAlContext();
        ~OpenAlContext();

        OpenAlContext(const OpenAlContext&) = delete;
        OpenAlContext& operator=(const OpenAlContext&) = delete;
        OpenAlContext(OpenAlContext&&) = delete;
        OpenAlContext& operator=(OpenAlContext&&) = delete;

        al::Listener& get_listener() { return listener; }

        static void destroy_openal_context();
    private:
        ALCdevice* device = nullptr;
        ALCcontext* context = nullptr;

        al::Listener listener;
    };
}
