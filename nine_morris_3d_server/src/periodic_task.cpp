#include "periodic_task.hpp"

void PeriodicTask::update() {
    if (!m_task) {
        return;
    }

    const auto time_now {std::chrono::system_clock::now()};
    auto elapsed {time_now - m_last_time};

    while (elapsed > m_period) {
        m_task();
        elapsed -= m_period;
        m_last_time = time_now;
    }
}
