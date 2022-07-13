#pragma once

#include "nine_morris_3d/board.h"

class MinimaxThread {
public:
    MinimaxThread() = default;
    MinimaxThread(Board* board)
        : board(board) {}
    ~MinimaxThread();

    MinimaxThread& operator=(MinimaxThread&& other);

    struct ResultPlace {
        size_t node_index = 0;
    };

    struct ResultTake {
        size_t node_index = 0;
    };

    struct ResultPutDown {
        size_t source_node_index = 0;
        size_t destination_node_index = 0;
    };

    using FunctionPlace = std::function<void(GamePosition, size_t*, std::atomic<bool>&)>;
    using FunctionTake = std::function<void(GamePosition, size_t*, std::atomic<bool>&)>;
    using FunctionPutDown = std::function<void(GamePosition, size_t*, size_t*, std::atomic<bool>&)>;

    void start_place(const FunctionPlace& function);
    void start_take(const FunctionTake& function);
    void start_put_down(const FunctionPutDown& function);

    bool is_running();
    void join();

    const ResultPlace& get_place_result() { return place_result; }
    const ResultTake& get_take_result() { return take_result; }
    const ResultPutDown& get_put_down_result() { return put_down_result; }
private:
    void join_threads();

    std::thread thread_place;
    std::thread thread_take;
    std::thread thread_put_down;
    std::atomic<bool> running = false;  // TODO this is dodgy

    ResultPlace place_result;
    ResultTake take_result;
    ResultPutDown put_down_result;

    Board* board = nullptr;
};
