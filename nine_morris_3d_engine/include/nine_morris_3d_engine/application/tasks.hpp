#pragma once

#include <vector>
#include <functional>

#include "nine_morris_3d_engine/application/id.hpp"

namespace sm {
    namespace internal {
        class TaskManager;
    }

    class Task {  // TODO on done function
    public:
        enum class Result {
            Done,
            Continue,
            Repeat
        };

        using TaskFunction = std::function<Result(const Task&)>;

        Task(Id id, const TaskFunction& function)
            : function(function), id(id) {}

        Id get_id() const { return id; }
        double get_total_time() const { return total_time; }
        unsigned int get_frames() const { return frames; }
    private:
        TaskFunction function;
        Id id;

        double total_time {};
        double start_time {};
        unsigned int frames {};

        friend class internal::TaskManager;
    };

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
