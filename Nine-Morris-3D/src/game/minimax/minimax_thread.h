#pragma once

#include "game/minimax/common.h"
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

    void start(MinimaxAlgorithm* minimax);

    bool is_running() const;
    void join();

    const Move& get_result() const { return result; }
private:
    void join_thread();

    std::thread thread;
    std::atomic<bool> running = false;

    Move result;
    Board* board = nullptr;
};
