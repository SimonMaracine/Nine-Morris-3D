#include "game/game.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

#include "game/post_processing/blur.hpp"
#include "game/global.hpp"

void create_post_processing(sm::Ctx& ctx) {
    sm::FramebufferSpecification specification;
    specification.width = ctx.get_window_width();
    specification.height = ctx.get_window_height();
    specification.color_attachments = {
        sm::Attachment(sm::AttachmentFormat::Rgba8, sm::AttachmentType::Texture)
    };

    ctx.global<Global>().blur_step = std::make_shared<BlurStep>(
        ctx.load_framebuffer("blur"_H, specification),
        ctx.load_shader(
            "blur"_H,
            ctx.path_engine_assets("shaders/internal/screen_quad.vert"),
            ctx.path_assets("shaders/post_processing/blur.frag")
        )
    );
}

void game_start(sm::Ctx&) {

}

void game_stop(sm::Ctx& ctx) {
    // This function is not called on error

    auto& g {ctx.global<Global>()};
    g.blur_step = nullptr;
}
