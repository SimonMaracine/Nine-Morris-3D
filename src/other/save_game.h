#pragma once

#include <entt/entt.hpp>

namespace save_game {
    void save_game(const entt::registry& registry);
    void load_game(entt::registry& registry);
}
