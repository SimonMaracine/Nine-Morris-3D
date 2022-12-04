#pragma once

#include "nine_morris_3d_engine/audio/openal/listener.h"

struct ALCdevice;
struct ALCcontext;

class OpenALContext {
public:
    OpenALContext();
    ~OpenALContext();

    OpenALContext(const OpenALContext&) = delete;
    OpenALContext& operator=(const OpenALContext&) = delete;
    OpenALContext(OpenALContext&&) = delete;
    OpenALContext& operator=(OpenALContext&&) = delete;

    al::Listener& get_listener() { return listener; }
private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;

    al::Listener listener;
};

void destroy_openal_context();
