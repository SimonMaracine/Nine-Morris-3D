#pragma once

void timer_update_system(entt::registry& registry, double time);
void timer_start_system(entt::registry& registry, double time);
void timer_stop_system(entt::registry& registry);
void timer_reset_last_time_system(entt::registry& registry, double time);
void timer_get_time_formatted_system(entt::registry& registry, char* formatted_time);  // 32 bytes is ok
void timer_set_time_system(entt::registry& registry, unsigned int time);
