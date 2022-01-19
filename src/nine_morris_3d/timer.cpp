#include "nine_morris_3d/timer.h"
#include "other/logging.h"

void Timer::update(double time) {
    static double total_time = 0.0;

    if (running) {
        const double elapsed_time = time - last_time;
        last_time = time;

        total_time += elapsed_time;

        if (total_time > 0.1) {
            this->time++;
            total_time = 0.0;
        }
    }
}

void Timer::start() {
    running = true;

    SPDLOG_DEBUG("Started timer");
}

void Timer::stop() {
    running = false;

    SPDLOG_DEBUG("Stopped timer");
}

unsigned int Timer::get_time_seconds() const {
    return time / 10;
}

unsigned int Timer::get_time_raw() const {
    return time;
}

void Timer::set_time(unsigned int time) {
    this->time = time;
}
