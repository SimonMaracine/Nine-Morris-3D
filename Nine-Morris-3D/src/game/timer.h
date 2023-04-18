#pragma once

#include <engine/public/application_base.h>

class Timer {
public:
    Timer() = default;
    Timer(Ctx* ctx)
        : ctx(ctx) {}
    Timer(Ctx* ctx, unsigned int time)
        : time(time), ctx(ctx) {}
    ~Timer() = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) noexcept = default;

    void update();
    void start();
    void stop();
    void reset_last_time();
    bool is_running() const;
    unsigned int get_time() const;
    std::string get_time_formatted() const;
private:
    unsigned int time = 0;  // In deciseconds
    double last_time = 0.0;  // Last GLFW time (in seconds)
    bool running = false;

    Ctx* ctx = nullptr;
};
