#pragma once

#include <chrono>
#include <tuple>

class Clock {
public:
    void reset(unsigned int time = 1000 * 60 * 10);
    void start();
    void stop();
    void update();
    void switch_turn();

    unsigned int get_white_time() const { return m_white_time; }
    unsigned int get_black_time() const { return m_black_time; }
    void set_white_time(unsigned int white_time) { m_white_time = white_time; }
    void set_black_time(unsigned int black_time) { m_black_time = black_time; }

    static std::tuple<unsigned int, unsigned int, unsigned int> split_time(unsigned int time);
    static unsigned int as_centiseconds(unsigned int minutes);
private:
    static void set_time(std::chrono::steady_clock::time_point& last_time);
    static void update_time(unsigned int& time, std::chrono::steady_clock::time_point& last_time);

    bool m_running {false};
    bool m_player_white {true};
    unsigned int m_white_time {1000 * 60 * 10};
    unsigned int m_black_time {1000 * 60 * 10};
    std::chrono::steady_clock::time_point m_white_last_time {};
    std::chrono::steady_clock::time_point m_black_last_time {};
};
