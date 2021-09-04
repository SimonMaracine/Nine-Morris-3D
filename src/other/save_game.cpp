#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "other/save_game.h"

#define SAVE_GAME_FILE "last_game.json"

using json = nlohmann::json;

namespace save_game {
    void save_game(const entt::registry& registry) {
    }

    void load_game(entt::registry& registry) {

    }
}
