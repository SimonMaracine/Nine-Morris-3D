#include "game.hpp"

#include "global.hpp"
#include "options.hpp"
#include "window_size.hpp"

void game_start(sm::Ctx& ctx) {
    auto& g {ctx.global<Global>()};

    try {
        load_options(g.options, ctx.path_saved_data(OPTIONS_FILE_NAME));
    } catch (const OptionsError& e) {
        // Must reset the data, as it may be corrupted
        g.options = Options();

        LOG_DIST_ERROR("Could not load options: {}", e.what());
    }

    ctx.set_window_vsync(g.options.vsync);

    // The window is still hidden at this point
    if (g.get_scale() == 2.0f) {
        ctx.set_window_size(DEFAULT_WIDTH_LARGE, DEFAULT_HEIGHT_LARGE);
    }

    sm::TexturePostProcessing post_processing;
    post_processing.flip = false;

    ctx.set_window_icons(
        {
            std::make_unique<sm::TextureData>(sm::utils::read_file(ctx.path_assets("icons/32x32/nine_morris_3d.png")), post_processing),
            std::make_unique<sm::TextureData>(sm::utils::read_file(ctx.path_assets("icons/64x64/nine_morris_3d.png")), post_processing),
            std::make_unique<sm::TextureData>(sm::utils::read_file(ctx.path_assets("icons/128x128/nine_morris_3d.png")), post_processing),
            std::make_unique<sm::TextureData>(sm::utils::read_file(ctx.path_assets("icons/256x256/nine_morris_3d.png")), post_processing),
            std::make_unique<sm::TextureData>(sm::utils::read_file(ctx.path_assets("icons/512x512/nine_morris_3d.png")), post_processing)
        }
    );

    // We said earlier to not initialize the renderer with default parameters
    sm::RendererSpecification specification;
    specification.samples = g.options.anti_aliasing;
    specification.scale = g.options.scale;
    specification.shadow_map_size = g.options.shadow_quality;

    ctx.initialize_renderer(specification);
}

void game_stop(sm::Ctx& ctx) {
    auto& g {ctx.global<Global>()};

    try {
        save_options(g.options, ctx.path_saved_data(OPTIONS_FILE_NAME));
    } catch (const OptionsError& e) {
        LOG_DIST_ERROR("Could not save options: {}", e.what());
    }
}
