#include "game/game.hpp"

#include "game/post_processing/blur.hpp"
#include "game/global.hpp"

void create_post_processing(sm::Ctx& ctx) {
    sm::FramebufferSpecification specification;
    specification.width = ctx.win.get_width();
    specification.height = ctx.win.get_height();
    specification.color_attachments = {
        sm::Attachment(sm::AttachmentFormat::Rgba8, sm::AttachmentType::Texture)
    };

    const auto framebuffer {std::make_shared<sm::GlFramebuffer>(specification)};

    ctx.rnd.register_framebuffer(framebuffer);

    ctx.global<Global>().blur_step = std::make_shared<BlurStep>(
        framebuffer,
        std::make_unique<sm::GlShader>(
            sm::utils::read_file(ctx.fs.path_engine_assets("shaders/screen_quad.vert")),
            sm::utils::read_file(ctx.fs.path_assets("shaders/post_processing/blur.frag"))
        )
    );
}

void game_start(sm::Ctx& ctx) {
    create_post_processing(ctx);
}
