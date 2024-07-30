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

        using TaskFunction = std::function<Result(const Task&, void*)>;

        Task(const TaskFunction& function, void* user_data)
            : m_function(function), m_user_data(user_data) {}

        double get_total_time() const noexcept {
            return m_total_time;
        }
    private:
        TaskFunction m_function;
        void* m_user_data {};
        double m_total_time {};
        double m_start_time {};

        friend class internal::TaskManager;
    };

    // Long-lived procedure executed on a separate thread, bound to the calling scene
    class AsyncTask {
    public:
        using TaskFunction = std::function<void(AsyncTask&, void*)>;

        AsyncTask(const TaskFunction& function, void* user_data)
            : m_thread(function, std::ref(*this), user_data) {}

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
