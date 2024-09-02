#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Timer {
public:
    Timer() = default;
    explicit Timer(std::shared_ptr<sm::Font> font);

    void start();
    void stop();

    bool is_running() const { return m_running; }
    unsigned int get_time() const { return m_time; }

    void update();
    void render(sm::Ctx& ctx);
private:
    bool m_running {false};
    unsigned int m_time {0};  // Tenths of seconds
    double m_last_time {0.0};  // Seconds
    double m_total_time {0.0};  // Seconds

    sm::Text m_text;
};
