#pragma once

class Timer {
public:
    Timer() = default;
    ~Timer() = default;

    void update(double time);
    void start(double time);
    void stop();
    unsigned int get_time_seconds() const;
    unsigned int get_time_raw() const;
    void get_time_formatted(char* formatted_time) const;  // 32 bytes is ok
    void set_time(unsigned int time);
private:
    unsigned int time = 0;  // In deciseconds
    double last_time = 0.0;  // Last GLFW time (in seconds)
    bool running = false;
};
