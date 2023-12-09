#pragma once

#include <vector>
#include <functional>

#include <resmanager/resmanager.hpp>

namespace sm {
    class Application;
    class TaskManager;

    class Task {  // TODO on done function
    public:
        enum class Result {
            Done,
            Continue,
            Again
        };

        using TaskId = resmanager::HashedStr64;
        using TaskFunction = std::function<Result(const Task&)>;

        Task(TaskId id, const TaskFunction& function)
            : function(function), id(id) {}
        ~Task() = default;

        Task(const Task&) = default;
        Task& operator=(const Task&) = default;
        Task(Task&&) noexcept = default;
        Task& operator=(Task&&) noexcept = default;

        TaskId get_id() const { return id; }
        double get_total_time() const { return total_time; }
        unsigned int get_frames() const { return frames; }
    private:
        TaskFunction function;
        TaskId id;

        double total_time {0.0};
        double start_time {0.0};
        unsigned int frames {0};

        friend class TaskManager;
    };

    class TaskManager {  // TODO async tasks, delayed tasks
    public:
        TaskManager() = default;
        ~TaskManager() = default;

        TaskManager(const TaskManager&) = delete;
        TaskManager& operator=(const TaskManager&) = delete;
        TaskManager(TaskManager&&) = delete;
        TaskManager& operator=(TaskManager&&) = delete;

        void add(Task::TaskId id, const Task::TaskFunction& function);
        void remove(Task::TaskId id);
    private:
        void update();

        std::vector<Task> tasks_active;
        std::vector<Task> tasks_next;

        friend class Application;
    };
}
