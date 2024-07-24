#include "nine_morris_3d_engine/application/internal/task_manager.hpp"

#include <algorithm>

#include "nine_morris_3d_engine/application/internal/window.hpp"

namespace sm {
    namespace internal {
        void TaskManager::add(const Task::TaskFunction& function, void* user_data) {
            std::lock_guard<std::mutex> lock {mutex};

            tasks_active.emplace_back(function, user_data);
        }

        void TaskManager::add_async(const AsyncTask::TaskFunction& function, void* user_data) {
            std::lock_guard<std::mutex> lock {async.mutex};

            async.tasks.push_back(std::make_unique<AsyncTask>(function, user_data));
        }

        void TaskManager::update() {
            update_tasks();
            update_async_tasks();
        }

        void TaskManager::wait_async() {
            for (const auto& async_task : async.tasks) {
                async_task->stop.store(true);
            }

            while (!async.tasks.empty()) {
                update_async_tasks();
            }
        }

        void TaskManager::update_tasks() {
            std::lock_guard<std::mutex> lock {mutex};

            for (Task& task : tasks_active) {
                if (task.start_time == 0.0) {
                    task.start_time = Window::get_time();
                } else {
                    task.total_time = Window::get_time() - task.start_time;
                }

                const Task::Result result {task.function(task, task.user_data)};

                switch (result) {
                    case Task::Result::Done:
                        break;
                    case Task::Result::Repeat:
                        tasks_next.push_back(task);
                        break;
                }
            }

            std::swap(tasks_active, tasks_next);
            tasks_next.clear();
        }

        void TaskManager::update_async_tasks() {
            std::lock_guard<std::mutex> lock {async.mutex};

            async.tasks.erase(
                std::remove_if(async.tasks.begin(), async.tasks.end(), [](const std::unique_ptr<AsyncTask>& async_task) {
                    return async_task->done.load();
                }),
                async.tasks.cend()
            );
        }
    }
}
