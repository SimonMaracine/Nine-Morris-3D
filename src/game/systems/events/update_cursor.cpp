#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/systems/events/update_cursor.h"
#include "game/components/options.h"
#include "game/components/board.h"

void UpdateCursorSystem::run_once(UpdateCursorEvent& e) {
    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto options_view = registry.view<OptionsComponent>();
    auto& options_c = options_view.get<OptionsComponent>(options_view.back());

    if (options_c.custom_cursor) {
        if (board_c.should_take_piece) {
            // app->window->set_cursor(app->cross_cursor);

            // if (board_c.keyboard != entt::null) {  // TODO use events
                // keyboard->quad.texture = app->data.keyboard_controls_cross_texture;
            // }
        } else {
            // app->window->set_cursor(app->arrow_cursor);

            // if (keyboard != nullptr) {
                // keyboard->quad.texture = app->data.keyboard_controls_texture;
            // }
        }
    }
}
