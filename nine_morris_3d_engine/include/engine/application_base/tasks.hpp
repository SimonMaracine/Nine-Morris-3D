#pragma once

#include <vector>
#include <functional>

#include "engine/application_base/id.hpp"

namespace sm {
    class Application;
    class TaskManager;

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

        friend class TaskManager;
    };

    class TaskManager {  // TODO async tasks, delayed tasks
    public:
        void add(Id id, const Task::TaskFunction& function);
        void remove(Id id);
    private:
        void update();

        std::vector<Task> tasks_active;
        std::vector<Task> tasks_next;

        friend class Application;
    };
}
