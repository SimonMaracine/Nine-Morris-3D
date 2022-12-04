#pragma once

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
private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
};

void destroy_openal_context();
