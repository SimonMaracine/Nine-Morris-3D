#pragma once

#include "game/game_position.h"
#include "other/constants.h"

struct Board;

class MinimaxThread {
public:
    MinimaxThread() = default;
    MinimaxThread(Board* board)
        : board(board) {}
    ~MinimaxThread();

    MinimaxThread(const MinimaxThread&) = delete;
    MinimaxThread& operator=(const MinimaxThread&) = delete;
    MinimaxThread(MinimaxThread&&) = delete;
    MinimaxThread& operator=(MinimaxThread&& other) noexcept;

    struct Result {
        size_t place_node_index = NULL_INDEX;
        size_t take_node_index = NULL_INDEX;
        size_t put_down_source_node_index = NULL_INDEX;
        size_t put_down_destination_node_index = NULL_INDEX;
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
