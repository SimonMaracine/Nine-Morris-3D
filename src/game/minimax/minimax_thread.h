#pragma once

#include "other/constants.h"

struct Board;

class MinimaxThread {
public:
    MinimaxThread() = default;
    MinimaxThread(Board* board)
        : board(board) {}
    ~MinimaxThread();

    MinimaxThread& operator=(MinimaxThread&& other);

    struct Result {
        size_t place_node_index = 0;
        size_t take_node_index = 0;
        size_t put_down_source_node_index = 0;
        size_t put_down_destination_node_index = 0;
    };

    using Function = std::function<void(GamePosition, Result&, std::atomic<bool>&)>;

    void start(const Function& function);

    bool is_running();
    void join();

    const Result& get_result() { return result; }
private:
    void join_thread();

    std::thread thread;
    std::atomic<bool> running = false;

    Result result;
    Board* board = nullptr;
};
