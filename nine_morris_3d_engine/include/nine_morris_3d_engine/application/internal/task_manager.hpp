#pragma once

#include <vector>

#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/application/task.hpp"

namespace sm {
    namespace internal {
        class TaskManager {  // TODO async tasks, delayed tasks
        public:
            void add(Id id, const Task::TaskFunction& function);
            void remove(Id id);

            void update();
        private:
            std::vector<Task> tasks_active;
            std::vector<Task> tasks_next;
        };
    }
}
