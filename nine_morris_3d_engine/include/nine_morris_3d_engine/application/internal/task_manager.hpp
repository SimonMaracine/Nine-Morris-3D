#pragma once

#include <vector>
#include <mutex>
#include <memory>

#include "nine_morris_3d_engine/application/task.hpp"

namespace sm::internal {
    class DebugUi;

    // Thread safe tasks API
    class TaskManager {
    public:
        // Enqueue a normal task
        void add_immediate(Task::Function&& function);

        // Enqueue a delayed task
        void add_delayed(Task::Function&& function, double delay);

        // Enqueue a deffered task (by one frame)
        void add_deffered(Task::Function&& function);

        // Enqueue an asynchronous task (running in a thread)
        void add_async(AsyncTask::Function&& function);

        // Execute tasks
        void update();

        // Join asynchronous tasks; throw the last exception (if any)
        void wait_async();
    private:
        void update_tasks();
        void update_async_tasks();

        std::vector<Task> m_tasks_active;  // Front list
        std::vector<Task> m_tasks_next;  // Back list
        std::mutex m_mutex;

        std::vector<std::unique_ptr<AsyncTask>> m_async_tasks;
        std::mutex m_async_mutex;

        friend class DebugUi;
    };
}
