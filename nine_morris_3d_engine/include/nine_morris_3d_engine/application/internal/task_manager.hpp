#pragma once

#include <vector>
#include <mutex>
#include <memory>

#include "nine_morris_3d_engine/application/task.hpp"

namespace sm::internal {
    class TaskManager {
    public:
        void add(const Task::TaskFunction& function, void* user_data = nullptr);
        void add_async(const AsyncTask::TaskFunction& function, void* user_data = nullptr);

        void update();
        void wait_async();
    private:
        void update_tasks();
        void update_async_tasks();

        std::vector<Task> tasks_active;
        std::vector<Task> tasks_next;
        std::mutex mutex;

        std::vector<std::unique_ptr<AsyncTask>> async_tasks;
        std::mutex async_mutex;
    };
}
