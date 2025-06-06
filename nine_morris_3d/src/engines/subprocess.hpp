#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <stdexcept>
#include <exception>

#include <nine_morris_3d_engine/external/process_asio.h++>

namespace boost_process = boost::process::v2;

class Subprocess {
public:
    Subprocess();
    ~Subprocess();

    Subprocess(const Subprocess&) = delete;
    Subprocess& operator=(const Subprocess&) = delete;
    Subprocess(Subprocess&&) = delete;
    Subprocess& operator=(Subprocess&&) = delete;

    void open(boost::filesystem::path executable, bool search_executable = false);
    void wait();
    bool alive();
    std::string read_line();
    void write_line(const std::string& data);
private:
    void throw_if_error();
    void kill();
    static std::string extract_line(std::string& read_buffer);
    void task_read_line();

    boost::asio::io_context m_context;
    boost::asio::readable_pipe m_out;
    boost::asio::writable_pipe m_in;
    boost_process::process m_process;
    std::thread m_context_thread;

    std::mutex m_read_mutex;
    std::string m_read_buffer;
    std::deque<std::string> m_reading_queue;

    std::exception_ptr m_exception;
};

struct SubprocessError : public std::runtime_error {
    explicit SubprocessError(const char* message)
        : std::runtime_error(message) {}
    explicit SubprocessError(const std::string& message)
        : std::runtime_error(message) {}
};
