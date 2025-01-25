#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <exception>

#include "nine_morris_3d_engine/application/error.hpp"

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

        using TaskFunction = std::function<Result()>;

        Task(const TaskFunction& function, double last_time, double delay)
            : m_function(function), m_last_time(last_time), m_delay(delay) {}
    private:
        TaskFunction m_function;
        double m_last_time {};
        double m_delay {};

        friend class internal::TaskManager;
    };

    // Long-lived procedure executed on a separate thread, bound to the calling scene
    class AsyncTask {
    public:
        using TaskFunction = std::function<void(AsyncTask&)>;

        AsyncTask(const TaskFunction& function)
            : m_thread(function, std::ref(*this)) {}

        ~AsyncTask() {
            m_thread.join();
        }

        AsyncTask(const AsyncTask&) = delete;
        AsyncTask& operator=(const AsyncTask&) = delete;
        AsyncTask(AsyncTask&&) = delete;
        AsyncTask& operator=(AsyncTask&&) = delete;

        void set_done() noexcept {
            m_done.store(true);
        }

        void set_done(std::exception_ptr exception) noexcept {
            m_exception = exception;
            m_done.store(true);
        }

        bool stop_requested() const noexcept {
            return m_stop.load();
        }
    private:
        std::thread m_thread;
        std::atomic_bool m_done {false};
        std::atomic_bool m_stop {false};  // Set by task manager
        std::exception_ptr m_exception;

        friend class internal::TaskManager;
    };
}
