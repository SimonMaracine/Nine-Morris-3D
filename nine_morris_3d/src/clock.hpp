#pragma once

#include <chrono>
#include <tuple>
#include <type_traits>

#include <protocol.hpp>

class Clock {
public:
    using Time = unsigned int;  // In milliseconds
    static constexpr unsigned int DEFAULT_TIME {1000 * 60 * 10};

    static_assert(std::is_same_v<Time, protocol::ClockTime>);

    void reset(Time time = DEFAULT_TIME);
    void start();
    void stop();
    void update();
    void switch_turn();

    Time get_white_time() const { return m_white_time; }
    Time get_black_time() const { return m_black_time; }
    void set_white_time(Time white_time) { m_white_time = white_time; }
    void set_black_time(Time black_time) { m_black_time = black_time; }

    // A rather strange API, but very needed...
    void set_white_time_point(Time white_time_point) { m_white_time_point = white_time_point; }
    void set_black_time_point(Time black_time_point) { m_black_time_point = black_time_point; }
    void reset_white_to_time_point() { m_white_time = m_white_time_point; }
    void reset_black_to_time_point() { m_black_time = m_black_time_point; }

    static std::tuple<Time, Time, Time> split_time(Time time);

    static constexpr Time as_milliseconds(Time minutes) {
        return 1000 * 60 * minutes;
    }

    static constexpr Time as_minutes(Time milliseconds) {
        return milliseconds / 1000 / 60;
    }
private:
    static void set_time(std::chrono::steady_clock::time_point& last_time);
    static void update_time(Time& time, std::chrono::steady_clock::time_point& last_time);

    bool m_running {false};
    bool m_player_white {true};
    Time m_white_time {DEFAULT_TIME};
    Time m_black_time {DEFAULT_TIME};
    Time m_white_time_point {DEFAULT_TIME};
    Time m_black_time_point {DEFAULT_TIME};
    std::chrono::steady_clock::time_point m_white_last_time {};
    std::chrono::steady_clock::time_point m_black_last_time {};
};
