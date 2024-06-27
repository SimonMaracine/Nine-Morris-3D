#include "game/game.hpp"

#include "post_processing/blur.hpp"

void setup_post_processing(sm::Ctx& ctx) {
    auto shader {std::make_unique<sm::GlShader>(
        sm::utils::read_file(ctx.fs.path_engine_assets("shaders/screen_quad.vert")),
        sm::utils::read_file(ctx.fs.path_assets("shaders/post_processing/blur.frag"))
    )};

    sm::FramebufferSpecification specification;
    specification.width = ctx.win.get_width();
    specification.height = ctx.win.get_height();
    specification.color_attachments = {
        sm::Attachment(sm::AttachmentFormat::Rgba8, sm::AttachmentType::Texture)
    };

    const auto framebuffer {std::make_shared<sm::GlFramebuffer>(specification)};

    ctx.rnd.register_framebuffer(framebuffer);

    ctx.rnd.add_post_processing<BlurStep>(framebuffer, std::move(shader));
}

void game_start(sm::Ctx& ctx) {
    setup_post_processing(ctx);
}
