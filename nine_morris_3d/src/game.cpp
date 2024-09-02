#include "game.hpp"

#include "global.hpp"
#include "options.hpp"

void create_post_processing(sm::Ctx&) {

}

void game_start(sm::Ctx& ctx) {
    auto& g {ctx.global<Global>()};

    try {
        load_options(g.options, ctx.path_saved_data(OPTIONS_FILE_NAME));
    } catch (const OptionsFileError& e) {
        LOG_DIST_ERROR("Could not load options: {}", e.what());
    }

    ctx.set_window_vsync(g.options.vsync ? 1 : 0);

    // The window is still hidden at this point
    if (g.get_scale() == 2.0f) {
        ctx.set_window_dimensions(DEFAULT_WIDTH_LARGE, DEFAULT_HEIGHT_LARGE);
    }
}

void game_stop(sm::Ctx& ctx) {
    const auto& g {ctx.global<Global>()};

    try {
        save_options(g.options, ctx.path_saved_data(OPTIONS_FILE_NAME));
    } catch (const OptionsFileError& e) {
        LOG_DIST_ERROR("Could not save options: {}", e.what());
    }
}
