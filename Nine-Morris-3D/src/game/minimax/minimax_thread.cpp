#include "game/minimax/minimax_thread.h"
#include "game/minimax/common.h"
#include "game/entities/board.h"
#include "other/constants.h"

MinimaxThread::~MinimaxThread() {
    join_thread();
}

MinimaxThread& MinimaxThread::operator=(MinimaxThread&& other) noexcept {
    join_thread();

    thread = std::move(other.thread);
    running.store(other.running.load());

    result = other.result;
    board = other.board;

    return *this;
}

void MinimaxThread::start(MinimaxAlgorithm* minimax) {
    running.store(true);
    result = Move {};

    auto position = board->get_position();
    const auto player = board->turn;

    thread = std::thread(  // TODO spawn only one thread and reuse it
        &MinimaxAlgorithm::start,
        minimax,
        position,
        static_cast<PieceType>(player),
        std::ref(result),
        std::ref(running)
    );
}

bool MinimaxThread::is_running() const {
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
