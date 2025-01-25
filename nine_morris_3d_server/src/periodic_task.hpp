#pragma once

#include <chrono>
#include <functional>
#include <utility>

class PeriodicTask {
public:
    PeriodicTask() = default;
    PeriodicTask(std::function<void()>&& task, std::chrono::system_clock::duration period)
        : m_task(std::move(task)), m_period(period), m_last_time(std::chrono::system_clock::now()) {}

    void update();
private:
    std::function<void()> m_task;
    std::chrono::system_clock::duration m_period;
    std::chrono::system_clock::time_point m_last_time;
};
