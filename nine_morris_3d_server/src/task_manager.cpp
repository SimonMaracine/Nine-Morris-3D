#include "task_manager.hpp"

void TaskManager::add_immediate(Task::Function&& function) {
    m_tasks_next.emplace_back(std::move(function), std::chrono::system_clock::now(), std::chrono::system_clock::duration::zero(), false);
}

void TaskManager::add_delayed(Task::Function&& function, Task::Duration delay) {
    m_tasks_next.emplace_back(std::move(function), std::chrono::system_clock::now(), delay, false);
}

void TaskManager::add_deffered(Task::Function&& function) {
    m_tasks_next.emplace_back(std::move(function), std::chrono::system_clock::now(), std::chrono::system_clock::duration::zero(), true);
}

void TaskManager::update() {
    for (Task& task : m_tasks_active) {
        if (task.m_defer) {
            task.m_defer = false;
            m_tasks_next.push_back(std::move(task));
            continue;
        }

        const auto time_now {std::chrono::system_clock::now()};

        if (task.m_delay > task.m_delay.zero()) {
            if (time_now - task.m_last_time < task.m_delay) {
                m_tasks_next.push_back(std::move(task));
                continue;
            }
        }

        switch (task.m_function()) {
            case Task::Result::Done:
                break;
            case Task::Result::Repeat:
                task.m_last_time = time_now;
                m_tasks_next.push_back(std::move(task));
                break;
        }
    }

    std::swap(m_tasks_active, m_tasks_next);
    m_tasks_next.clear();
}
