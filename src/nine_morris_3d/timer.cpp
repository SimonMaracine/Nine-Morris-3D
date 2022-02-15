#include <string.h>

#include "nine_morris_3d/timer.h"
#include "other/logging.h"

void Timer::update(double time) {
    static double total_time = 0.0;

    if (running) {
        const double elapsed_time = time - last_time;
        last_time = time;

        total_time += elapsed_time;

        while (true) {
            if (total_time > 0.1) {
                total_time -= 0.1;
                this->time++;
            } else {
                break;
            }
        }
    }
}

void Timer::start(double time) {
    running = true;
    last_time = time;

    DEB_DEBUG("Started timer");
}

void Timer::stop() {
    running = false;

    DEB_DEBUG("Stopped timer");
}

unsigned int Timer::get_time_seconds() const {
    return time / 10;
}

unsigned int Timer::get_time_raw() const {
    return time;
}

void Timer::get_time_formatted(char* formatted_time) const {
    const unsigned int minutes = get_time_seconds() / 60;
    const unsigned int seconds = get_time_seconds() % 60;    

    sprintf(formatted_time, "%.2u:%.2u", minutes, seconds);
}

void Timer::set_time(unsigned int time) {
    this->time = time;
}
