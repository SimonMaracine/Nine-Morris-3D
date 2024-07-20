#include "nine_morris_3d_engine/application/tasks.hpp"

#include <algorithm>

#include "nine_morris_3d_engine/application/window.hpp"

namespace sm {
    namespace internal {
        void TaskManager::add(Id id, const Task::TaskFunction& function) {
            tasks_active.emplace_back(id, function);
        }

        void TaskManager::remove(Id id) {
            tasks_active.erase(
                std::find_if(tasks_active.cbegin(), tasks_active.cend(), [this, id](const Task& task) {
                    return task.id == id;
                })
            );
        }

        void TaskManager::update() {
            for (Task& task : tasks_active) {
                if (task.start_time == 0.0) {
                    task.start_time = Window::get_time();
                } else {
                    task.total_time = Window::get_time() - task.start_time;
                }

                task.frames++;

                const Task::Result result {task.function(task)};

                switch (result) {
                    case Task::Result::Done:
                        break;
                    case Task::Result::Continue:
                        tasks_next.push_back(task);
                        break;
                    case Task::Result::Repeat:
                        // FIXME
                        break;
                }
            }

            tasks_active.clear();
            std::swap(tasks_active, tasks_next);
        }
    }
}
