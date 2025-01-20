#pragma once

#include <string>
#include <optional>
#include <vector>
#include <functional>
#include <fstream>
#include <stdexcept>

#include "engines/subprocess.hpp"

class Engine {
public:
    struct Info {};

    virtual ~Engine() = default;

    virtual void initialize(const std::string& file_path) = 0;
    virtual void set_debug(bool active) = 0;
    virtual void synchronize() = 0;
    virtual void set_option(const std::string& name, const std::optional<std::string>& value) = 0;
    virtual void new_game() = 0;
    virtual void start_thinking(
        const std::optional<std::string>& position,
        const std::vector<std::string>& moves,
        std::optional<unsigned int> wtime,
        std::optional<unsigned int> btime,
        std::optional<unsigned int> max_time
    ) = 0;
    virtual void stop_thinking() = 0;
    virtual std::optional<std::string> done_thinking() = 0;
    virtual void uninitialize() = 0;

    virtual bool is_null_move(const std::string& move) const = 0;

    void set_info_callback(std::function<void(const Info&)>&& info_callback);
    void set_log_output(bool enable, const std::string& file_path);
    const std::string& get_name() const { return m_name; }
protected:
    static std::vector<std::string> parse_message(const std::string& message);
    static bool token_available(const std::vector<std::string>& tokens, std::size_t index);

    Subprocess m_subprocess;
    std::function<void(const Info&)> m_info_callback;
    std::ofstream m_log_output_stream;
    std::string m_name;
};

struct EngineError : std::runtime_error {
    explicit EngineError(const char* message)
        : std::runtime_error(message) {}
    explicit EngineError(const std::string& message)
        : std::runtime_error(message) {}
};
