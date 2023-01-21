#pragma once

#include <engine/engine_application.h>

class Timer {
public:
    Timer() = default;
    Timer(Application* app)
        : app(app) {}
    Timer(Application* app, unsigned int time)
        : time(time), app(app) {}
    ~Timer() = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = default;

    void update();
    void start();
    void stop();
    void reset_last_time();
    bool is_running() const;
    unsigned int get_time() const;
    void get_time_formatted(char* out_formatted_time) const;  // Maximum 32 bytes
private:
    unsigned int time = 0;  // In deciseconds
    double last_time = 0.0;  // Last GLFW time (in seconds)
    bool running = false;

    Application* app = nullptr;
};
