#include <engine/engine_other.h>

#include "game/timer.h"

void Timer::update() {
    static double total_time = 0.0;

    const double current_time = app->window->get_time();

    if (running) {
        const double elapsed_time = current_time - last_time;
        last_time = current_time;

        total_time += elapsed_time;

        while (true) {
            if (total_time > 0.1) {
                total_time -= 0.1;
                time++;
            } else {
                break;
            }
        }
    }
}

void Timer::start() {
    ASSERT(!running, "Cannot start, if it is already running");

    const double current_time = app->window->get_time();

    running = true;
    last_time = current_time;

    DEB_DEBUG("Started timer");
}

void Timer::stop() {
    ASSERT(running, "Cannot stop, if it is already stopped");

    running = false;

    DEB_DEBUG("Stopped timer");
}

void Timer::reset_last_time() {
    const double current_time = app->window->get_time();

    last_time = current_time;
}

bool Timer::is_running() const {
    return running;
}

unsigned int Timer::get_time() const {
    return time;
}

std::string Timer::get_time_formatted() const {
    char formatted_time[32];

    const unsigned int time_in_seconds = time / 10;
    const unsigned int minutes = time_in_seconds / 60;
    const unsigned int seconds = time_in_seconds % 60;

    snprintf(formatted_time, 32, "%.2u:%.2u", minutes, seconds);

    return std::string(formatted_time);
}
