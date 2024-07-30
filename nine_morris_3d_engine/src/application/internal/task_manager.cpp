#include "nine_morris_3d_engine/application/internal/task_manager.hpp"

#include <algorithm>
#include <exception>

#include "nine_morris_3d_engine/application/internal/window.hpp"
#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm::internal {
    void TaskManager::add(const Task::TaskFunction& function, void* user_data) {
        std::lock_guard<std::mutex> lock {m_mutex};

        m_tasks_active.emplace_back(function, user_data);
    }

    void TaskManager::add_async(const AsyncTask::TaskFunction& function, void* user_data) {
        std::lock_guard<std::mutex> lock {m_async_mutex};

        m_async_tasks.push_back(std::make_unique<AsyncTask>(function, user_data));
    }

    void TaskManager::update() {
        {
            std::lock_guard<std::mutex> lock {m_mutex};

            update_tasks();
        }

        {
            std::lock_guard<std::mutex> lock {m_async_mutex};

            update_async_tasks();
        }
    }

    void TaskManager::wait_async() {
        std::lock_guard<std::mutex> lock {m_async_mutex};

        for (const auto& async_task : m_async_tasks) {
            async_task->m_stop.store(true);
        }

        std::exception_ptr last_exception;

        while (!m_async_tasks.empty()) {
            try {
                update_async_tasks();
            } catch (const OtherError&) {
                last_exception = std::current_exception();
            }
        }

        if (last_exception) {
            std::rethrow_exception(last_exception);
        }
    }

    void TaskManager::update_tasks() {
        for (Task& task : m_tasks_active) {
            if (task.m_start_time == 0.0) {
                task.m_start_time = Window::get_time();
            } else {
                task.m_total_time = Window::get_time() - task.m_start_time;
            }

            const Task::Result result {task.m_function(task, task.m_user_data)};

            switch (result) {
                case Task::Result::Done:
                    break;
                case Task::Result::Repeat:
                    m_tasks_next.push_back(task);
                    break;
            }
        }

        std::swap(m_tasks_active, m_tasks_next);
        m_tasks_next.clear();
    }

    void TaskManager::update_async_tasks() {
        bool tasks_done {false};
        std::exception_ptr exception;

        for (const auto& async_task : m_async_tasks) {
            if (async_task->m_done.load()) {
                if (async_task->m_exception) {
                    exception = async_task->m_exception;
                }

                tasks_done = true;
            }
        }

        if (tasks_done) {
            m_async_tasks.erase(
                std::remove_if(m_async_tasks.begin(), m_async_tasks.end(), [](const std::unique_ptr<AsyncTask>& async_task) {
                    return async_task->m_done.load();
                }),
                m_async_tasks.cend()
            );
        }

        if (exception) {
            try {
                std::rethrow_exception(exception);
            } catch (const RuntimeError& e) {
                SM_THROW_ERROR(OtherError, "An error occurred inside an async task: {}", e.what());
            }
        }
    }
}
