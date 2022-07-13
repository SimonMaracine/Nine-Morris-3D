#include "nine_morris_3d/minimax/minimax_thread.h"

MinimaxThread::~MinimaxThread() {
    join_threads();
}

MinimaxThread& MinimaxThread::operator=(MinimaxThread&& other) {
    join_threads();

    thread_place = std::move(other.thread_place);
    thread_take = std::move(other.thread_take);
    thread_put_down = std::move(other.thread_put_down);
    running.store(other.running.load());

    place_result = other.place_result;
    take_result = other.take_result;
    put_down_result = other.put_down_result;

    board = other.board;

    return *this;
}

void MinimaxThread::start_place(const FunctionPlace& function) {
    running.store(true);
    memset(&place_result, 0, sizeof(ResultPlace));

    thread_place = std::thread(
        function, board->get_position(), &place_result.node_index, std::ref(running)
    );
}

void MinimaxThread::start_take(const FunctionTake& function) {
    running.store(true);
    memset(&take_result, 0, sizeof(ResultTake));
    thread_take = std::thread(
        function, board->get_position(), &take_result.node_index, std::ref(running)
    );
}

void MinimaxThread::start_put_down(const FunctionPutDown& function) {
    running.store(true);
    memset(&put_down_result, 0, sizeof(ResultPutDown));
    thread_put_down = std::thread(
        function, board->get_position(), &put_down_result.source_node_index,
        &put_down_result.destination_node_index, std::ref(running)
    );
}

bool MinimaxThread::is_running() {
    return running.load();
}

void MinimaxThread::join() {
    join_threads();
}

void MinimaxThread::join_threads() {
    if (thread_place.joinable()) {
        thread_place.join();
    }

    if (thread_take.joinable()) {
        thread_take.join();
    }

    if (thread_put_down.joinable()) {
        thread_put_down.join();
    }
}
