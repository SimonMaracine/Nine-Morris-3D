#pragma once

#include <functional>
#include <thread>
#include <atomic>

namespace sm {
    namespace internal {
        class TaskManager;
    }

    // Short-lived procedure executed in the main loop
    class Task {
    public:
        enum class Result {
            Done,
            Repeat
        };

        using TaskFunction = std::function<Result(const Task&, void*)>;

        Task(const TaskFunction& function, void* user_data)
            : function(function), user_data(user_data) {}

        double get_total_time() const { return total_time; }
    private:
        TaskFunction function;
        void* user_data {nullptr};
        double total_time {};
        double start_time {};

        friend class internal::TaskManager;
    };

    // Long-lived procedure executed on a separate thread, bound to the calling scene
    class AsyncTask {
    public:
        using TaskFunction = std::function<void(AsyncTask&, void*)>;

        AsyncTask(const TaskFunction& function, void* user_data)
            : thread(function, std::ref(*this), user_data) {}

        ~AsyncTask() {
            thread.join();
        }

        AsyncTask(const AsyncTask&) = delete;
        AsyncTask& operator=(const AsyncTask&) = delete;
        AsyncTask(AsyncTask&&) = delete;
        AsyncTask& operator=(AsyncTask&&) = delete;

        void set_done() { done.store(true); }
        bool stop_requested() const { return stop.load(); }
    private:
        std::thread thread;
        std::atomic_bool done {false};
        std::atomic_bool stop {false};

        friend class internal::TaskManager;
    };
}
