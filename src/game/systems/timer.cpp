#include <entt/entt.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/components/timer.h"
#include "game/systems/timer.h"
// #include "other/logging.h"
// #include "other/assert.h"

void timer_update_system(entt::registry& registry, double time) {
    auto view = registry.view<TimerComponent>();
    auto& timer_c = view.get<TimerComponent>(view.back());

    static double total_time = 0.0;

    if (timer_c.running) {
        const double elapsed_time = time - timer_c.last_time;
        timer_c.last_time = time;

        total_time += elapsed_time;

        while (true) {
            if (total_time > 0.1) {
                total_time -= 0.1;
                timer_c.time++;
            } else {
                break;
            }
        }
    }
}

void timer_start_system(entt::registry& registry, double time) {
    auto view = registry.view<TimerComponent>();
    auto& timer_c = view.get<TimerComponent>(view.back());

    ASSERT(!timer_c.running, "Cannot start, if it is already running");

    timer_c.running = true;
    timer_c.last_time = time;

    DEB_DEBUG("Started timer");
}

void timer_stop_system(entt::registry& registry) {
    auto view = registry.view<TimerComponent>();
    auto& timer_c = view.get<TimerComponent>(view.back());

    timer_c.running = false;

    DEB_DEBUG("Stopped timer");
}

void timer_reset_last_time_system(entt::registry& registry, double time) {
    auto view = registry.view<TimerComponent>();
    auto& timer_c = view.get<TimerComponent>(view.back());

    timer_c.last_time = time;
}

void timer_get_time_formatted_system(entt::registry& registry, char* formatted_time) {
    auto view = registry.view<TimerComponent>();
    auto& timer_c = view.get<TimerComponent>(view.back());

    const unsigned int minutes = (timer_c.time / 10) / 60;
    const unsigned int seconds = (timer_c.time / 10) % 60;    

    sprintf(formatted_time, "%.2u:%.2u", minutes, seconds);
}

void timer_set_time_system(entt::registry& registry, unsigned int time) {
    auto view = registry.view<TimerComponent>();
    auto& timer_c = view.get<TimerComponent>(view.back());

    timer_c.time = time;
}
