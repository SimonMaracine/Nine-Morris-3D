#include "game.hpp"

#include "global.hpp"
#include "options.hpp"
#include "window_size.hpp"

#include "nine_morris_3d_engine/external/resmanager.h++"

void game_start(sm::Ctx& ctx) {
    auto& g {ctx.global<Global>()};

    sm::localization::load_catalog(ctx.path_assets("text/catalog.json"));

    try {
        load_options(g.options, ctx.path_saved_data(OPTIONS_FILE_NAME));
    } catch (const OptionsError& e) {
        // Must reset the data, as it may be corrupted
        g.options = Options();

        LOG_DIST_ERROR("Could not load options: {}", e.what());
    }

    switch (g.options.language) {
        case LanguageEnglish:
            sm::localization::set_language("en"_H);
            break;
        case LanguageRomanian:
            sm::localization::set_language("ro"_H);
            break;
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
