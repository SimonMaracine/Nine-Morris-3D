#pragma once

#include "engine/audio/openal/listener.h"

struct ALCdevice;
struct ALCcontext;

class OpenAlContext {
public:
    OpenAlContext();
    ~OpenAlContext();

    OpenAlContext(const OpenAlContext&) = delete;
    OpenAlContext& operator=(const OpenAlContext&) = delete;
    OpenAlContext(OpenAlContext&&) = delete;
    OpenAlContext& operator=(OpenAlContext&&) = delete;

    al::Listener& get_listener() { return listener; }
private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;

    al::Listener listener;
};

void destroy_openal_context();
