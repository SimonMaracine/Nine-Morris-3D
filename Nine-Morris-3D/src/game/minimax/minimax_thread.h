#pragma once

#include "other/constants.h"

struct Board;

class MinimaxThread {
public:
    MinimaxThread() = default;
    MinimaxThread(Board* board)
        : board(board) {}
    ~MinimaxThread();

    MinimaxThread(const MinimaxThread& other) = delete;
    MinimaxThread& operator=(const MinimaxThread& other) = delete;
    MinimaxThread(MinimaxThread&& other) = delete;
    MinimaxThread& operator=(MinimaxThread&& other) noexcept;

    struct Result {
        size_t place_node_index = 0;
        size_t take_node_index = 0;
        size_t put_down_source_node_index = 0;
        size_t put_down_destination_node_index = 0;
    };

    using Algorithm = std::function<void(GamePosition, Result&, std::atomic<bool>&)>;

    void start(const Algorithm& algorithm);

    bool is_running() const;
    void join();

    const Result& get_result() const { return result; }
private:
    void join_thread();

    std::thread thread;
    std::atomic<bool> running = false;

    Result result;
    Board* board = nullptr;
};
