#pragma once

#include <vector>
#include <chrono>

#include <functional>
#include <utility>

class TaskManager;

class Task {
public:
    enum class Result {
        Done,
        Repeat
    };

    using Function = std::function<Result()>;
    using TimePoint = std::chrono::system_clock::time_point;
    using Duration = std::chrono::system_clock::duration;

    Task(Function&& function, TimePoint last_time, Duration delay, bool defer)
        : m_function(std::move(function)), m_last_time(last_time), m_delay(delay), m_defer(defer) {}
private:
    Function m_function;
    TimePoint m_last_time {};
    Duration m_delay {};
    bool m_defer {};

    friend class TaskManager;
};

class TaskManager {
public:
    void add_immediate(Task::Function&& function);
    void add_delayed(Task::Function&& function, Task::Duration delay);
    void add_deffered(Task::Function&& function);

    void update();
private:
    std::vector<Task> m_tasks_active;
    std::vector<Task> m_tasks_next;
};
