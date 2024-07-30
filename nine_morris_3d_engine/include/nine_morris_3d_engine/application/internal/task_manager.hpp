#pragma once

#include <vector>
#include <mutex>
#include <memory>

#include "nine_morris_3d_engine/application/task.hpp"

namespace sm::internal {
    class DebugUi;

    class TaskManager {
    public:
        void add(const Task::TaskFunction& function, void* user_data = nullptr);
        void add_async(const AsyncTask::TaskFunction& function, void* user_data = nullptr);

        void update();
        void wait_async();
    private:
        void update_tasks();
        void update_async_tasks();

        std::vector<Task> m_tasks_active;
        std::vector<Task> m_tasks_next;
        std::mutex m_mutex;

        std::vector<std::unique_ptr<AsyncTask>> m_async_tasks;
        std::mutex m_async_mutex;

        friend class DebugUi;
    };
}
