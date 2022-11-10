#include "game/minimax/minimax_thread.h"
#include "game/entities/board.h"

MinimaxThread::~MinimaxThread() {
    join_thread();
}

MinimaxThread& MinimaxThread::operator=(MinimaxThread&& other) {
    join_thread();

    thread = std::move(other.thread);
    running.store(other.running.load());

    result = other.result;
    board = other.board;

    return *this;
}

void MinimaxThread::start(const Function& function) {
    running.store(true);
    result = Result {};

    thread = std::thread(function, board->get_position(), std::ref(result), std::ref(running));
}

bool MinimaxThread::is_running() {
    return running.load();
}

void MinimaxThread::join() {
    join_thread();
}

void MinimaxThread::join_thread() {
    if (thread.joinable()) {
        thread.join();
    }
}
