#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <exception>
#include <utility>

#include "nine_morris_3d_engine/application/error.hpp"

namespace sm {
    namespace internal {
        class TaskManager;
    }

    // Short-lived procedure executed in the main loop
    // May be instantiated from any thread
    // May be deffered by one frame and/or may be delayed until a certain amount of time
    // Delays are repeated, but defers are not
    class Task {
    public:
        enum class Result {
            Done,
            Repeat
        };

        using Function = std::function<Result()>;

        Task(Function&& function, double last_time, double delay, bool defer)
            : m_function(std::move(function)), m_last_time(last_time), m_delay(delay), m_defer(defer) {}
    private:
        Function m_function;
        double m_last_time {};
        double m_delay {};  // Time until the task is delayed
        bool m_defer {};  // If the task should be deffered until the next update

        friend class internal::TaskManager;
    };

    // Long-lived procedure executed on a separate thread, bound to the calling scene
    class AsyncTask {
    public:
        using Function = std::function<void(AsyncTask&)>;

        explicit AsyncTask(Function&& function)
            : m_thread(std::move(function), std::ref(*this)) {}

        ~AsyncTask() {
            m_thread.join();
        }

        AsyncTask(const AsyncTask&) = delete;
        AsyncTask& operator=(const AsyncTask&) = delete;
        AsyncTask(AsyncTask&&) = delete;
        AsyncTask& operator=(AsyncTask&&) = delete;

        // Signal that this task has finished successfully
        // Must be called
        void set_done() noexcept {
            m_done.store(true);
        }

        // Signal that this task has finished with an error
        // Must be called
        void set_done(std::exception_ptr exception) noexcept {
            m_exception = exception;
            m_done.store(true);
        }

        // Find out if the application wants to stop this task (ex: the application is closing)
        // Used by the user to prematurely stop a task, if possible, in order to avoid stalling the application
        bool stop_requested() const noexcept {
            return m_stop.load();
        }
    private:
        std::thread m_thread;
        std::atomic_bool m_done {false};  // Must be set by the user before the task finishes
        std::atomic_bool m_stop {false};  // Set by task manager
        std::exception_ptr m_exception;

        friend class internal::TaskManager;
    };
}
