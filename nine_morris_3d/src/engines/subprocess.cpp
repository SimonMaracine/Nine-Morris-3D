#include "subprocess.hpp"

#include <utility>
#include <cassert>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#ifdef SM_PLATFORM_LINUX
    #include <boost/process/posix/vfork_launcher.hpp>
#endif

Subprocess::Subprocess()
    : m_out(m_context), m_in(m_context), m_process(m_context) {}

Subprocess::~Subprocess() {
    kill();

    if (m_context_thread.joinable()) {
        m_context_thread.join();
    }
}

void Subprocess::open(const std::string& file_path) {
    if (m_context.stopped()) {
        m_context.restart();
    }

    try {
#ifdef SM_PLATFORM_LINUX
        const auto path {boost_process::environment::find_executable(file_path)};
        auto launcher {boost_process::posix::vfork_launcher()};
        m_process = launcher(m_context, path, std::vector<std::string> {}, boost_process::process_stdio{m_in, m_out, nullptr});
#else
        m_process = boost_process::process(m_context, file_path, {}, boost_process::process_stdio{m_in, m_out, nullptr});
#endif
    } catch (const boost_process::system_error& e) {
        throw SubprocessError(e.what());
    }

    task_read_line();

    m_context_thread = std::thread([this]() {
        try {
            m_context.run();
        } catch (...) {
            m_exception = std::current_exception();
        }
    });
}

void Subprocess::wait() {
    boost_process::error_code ec;
    m_process.wait(ec);

    if (m_context_thread.joinable()) {
        m_context_thread.join();
    }

    m_exception = nullptr;

    if (ec) {
        throw SubprocessError(ec.message());
    }
}

bool Subprocess::alive() {
    throw_if_error();

    boost_process::error_code ec;
    const bool result {m_process.running(ec)};

    if (ec) {
        throw SubprocessError(ec.message());
    }

    return result;
}

std::string Subprocess::read_line() {
    throw_if_error();

    {
        std::lock_guard lock {m_read_mutex};
        if (!m_reading_queue.empty()) {
            const auto result {m_reading_queue.front()};
            m_reading_queue.pop_front();
            return result;
        }
    }

    return {};
}

void Subprocess::write_line(const std::string& data) {
    throw_if_error();

    const auto line {data + '\n'};

    boost_process::error_code ec;
    boost::asio::write(m_in, boost::asio::const_buffer(line.data(), line.size()), ec);

    if (ec) {
        throw SubprocessError(ec.message());
    }
}

void Subprocess::throw_if_error() {
    if (m_exception) {
        try {
            std::rethrow_exception(std::exchange(m_exception, nullptr));
        } catch (const SubprocessError&) {
            throw;
        } catch (...) {
            throw SubprocessError("Unknown error");
        }
    }
}

void Subprocess::kill() {
    boost_process::error_code ec;
    m_process.terminate(ec);
}

std::string Subprocess::extract_line(std::string& read_buffer) {
    const std::size_t position {read_buffer.find('\n')};

    assert(position != std::string::npos);

    const auto result {read_buffer.substr(0, position)};
    read_buffer = read_buffer.substr(position + 1);
    return result;
}

void Subprocess::task_read_line() {
    boost::asio::async_read_until(m_out, boost::asio::dynamic_buffer(m_read_buffer), '\n', [this](boost_process::error_code ec, std::size_t) {
        if (ec) {
            throw SubprocessError(ec.message());
        }

        {
            std::lock_guard lock {m_read_mutex};
            m_reading_queue.push_back(extract_line(m_read_buffer));
        }

        task_read_line();
    });
}
