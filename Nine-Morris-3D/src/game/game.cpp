#include "engine/public/prelude.h"
#include <engine/public/graphics.h>
#include <engine/public/audio.h>
#include <engine/public/other.h>
#include <engine/public/external/resmanager.h++>

#include "game/assets.h"
#include "game/game_options.h"
#include "game/post_processing/bright_filter.h"
#include "game/post_processing/blur.h"
#include "game/post_processing/combine.h"
#include "launcher/launcher_options.h"
#include "other/options.h"
#include "other/data.h"
#include "other/options_gracefully.h"

static void load_game_options(sm::Ctx* ctx) {
    auto& data = ctx->data<Data>();

    options_gracefully::load_from_file<game_options::GameOptions>(
        game_options::GAME_OPTIONS_FILE, data.options, game_options::validate_load
    );
}

static void setup_icons(sm::Ctx* ctx) {
    using namespace sm::file_system;
    using namespace assets;

    const auto icons = {
        std::make_unique<sm::TextureData>(path_for_assets(ICON_512), false),
        std::make_unique<sm::TextureData>(path_for_assets(ICON_256), false),
        std::make_unique<sm::TextureData>(path_for_assets(ICON_128), false),
        std::make_unique<sm::TextureData>(path_for_assets(ICON_64), false),
        std::make_unique<sm::TextureData>(path_for_assets(ICON_32), false)
    };

    ctx->window->set_icons(icons);
}

static void setup_cursors(sm::Ctx* ctx) {
    using namespace sm::file_system;
    using namespace assets;

    ctx->window->add_cursor(
        "arrow"_H,
        std::make_unique<sm::TextureData>(sm::Encrypt::encr(path_for_assets(ARROW_CURSOR)), false),
        4, 1
    );

    ctx->window->add_cursor(
        "cross"_H,
        std::make_unique<sm::TextureData>(sm::Encrypt::encr(path_for_assets(CROSS_CURSOR)), false),
        8, 8
    );
}

static void setup_imgui_fonts(sm::Ctx* ctx) {
    auto& data = ctx->data<Data>();

    using namespace sm::file_system;
    using namespace assets;

    ImGuiIO& io = ImGui::GetIO();

    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"ă");
    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);

    io.FontDefault = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_SEMIBOLD_FONT).c_str(), 21.0f);
    data.imgui_info_font = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_SEMIBOLD_FONT).c_str(), 15.0f);
    data.imgui_windows_font = (
        io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_SEMIBOLD_FONT).c_str(), 24.0f, nullptr, ranges.Data)
    );
    io.Fonts->Build();
}

static void setup_game_fonts(sm::Ctx* ctx) {
    using namespace sm::file_system;
    using namespace assets;

    {
        auto font = ctx->res.font.load(
            "good_dog_plain"_H, path_for_assets(GOOD_DOG_PLAIN_FONT), 50.0f, 6, 180, 40, 512
        );

        font->begin_baking();
        font->bake_ascii();
        font->end_baking();
    }

    {
        auto font = ctx->res.font.load(
            "open_sans"_H, path_for_assets(OPEN_SANS_SEMIBOLD_FONT), 50.0f, 6, 180, 40, 512
        );

        font->begin_baking();
        font->bake_ascii();
        font->end_baking();
    }
}

static void setup_post_processing(sm::Ctx* ctx) {
    auto& data = ctx->data<Data>();

    if (!data.launcher_options.bloom) {
        return;  // Disable bloom
    }

    using namespace sm::file_system;
    using namespace assets;

    {
        sm::gl::FramebufferSpecification specification;
        specification.width = ctx->properties->width / 2;
        specification.height = ctx->properties->height / 2;
        specification.resize_divisor = 2;
        specification.color_attachments = {
            sm::gl::Attachment {sm::gl::AttachmentFormat::Rgba8, sm::gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<sm::gl::Framebuffer>(specification);

        ctx->purge_framebuffers();
        ctx->add_framebuffer(framebuffer);

        auto shader = std::make_shared<sm::gl::Shader>(
            sm::Encrypt::encr(path_for_assets(BRIGHT_FILTER_VERTEX_SHADER)),
            sm::Encrypt::encr(path_for_assets(BRIGHT_FILTER_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );

        ctx->r3d->add_post_processing(std::make_unique<BrightFilter>("bright_filter", framebuffer, shader));
    }

    auto blur_shader = std::make_shared<sm::gl::Shader>(
        sm::Encrypt::encr(path_for_assets(BLUR_VERTEX_SHADER)),
        sm::Encrypt::encr(path_for_assets(BLUR_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_screen_texture" }
    );

    {
        sm::gl::FramebufferSpecification specification;
        specification.width = ctx->properties->width / 4;
        specification.height = ctx->properties->height / 4;
        specification.resize_divisor = 4;
        specification.color_attachments = {
            sm::gl::Attachment {sm::gl::AttachmentFormat::Rgba8, sm::gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<sm::gl::Framebuffer>(specification);

        ctx->purge_framebuffers();
        ctx->add_framebuffer(framebuffer);

        ctx->r3d->add_post_processing(std::make_unique<Blur>("blur1", framebuffer, blur_shader));
    }
    {
        sm::gl::FramebufferSpecification specification;
        specification.width = ctx->properties->width / 8;
        specification.height = ctx->properties->height / 8;
        specification.resize_divisor = 8;
        specification.color_attachments = {
            sm::gl::Attachment {sm::gl::AttachmentFormat::Rgba8, sm::gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<sm::gl::Framebuffer>(specification);

        ctx->purge_framebuffers();
        ctx->add_framebuffer(framebuffer);

        ctx->r3d->add_post_processing(std::make_unique<Blur>("blur2", framebuffer, blur_shader));
    }
    {
        sm::gl::FramebufferSpecification specification;
        specification.width = ctx->properties->width;
        specification.height = ctx->properties->height;
        specification.color_attachments = {
            sm::gl::Attachment {sm::gl::AttachmentFormat::Rgba8, sm::gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<sm::gl::Framebuffer>(specification);

        auto shader = std::make_shared<sm::gl::Shader>(
            sm::Encrypt::encr(path_for_assets(COMBINE_VERTEX_SHADER)),
            sm::Encrypt::encr(path_for_assets(COMBINE_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture", "u_bright_texture", "u_strength" }
        );

        ctx->purge_framebuffers();
        ctx->add_framebuffer(framebuffer);

        auto combine = std::make_unique<Combine>("combine", framebuffer, shader);
        combine->strength = data.launcher_options.bloom_strength;
        ctx->r3d->add_post_processing(std::move(combine));
    }
}

namespace game {
    void start(sm::Ctx* ctx) {
        auto& data = ctx->data<Data>();

        load_game_options(ctx);
        setup_icons(ctx);
        setup_cursors(ctx);
        setup_imgui_fonts(ctx);
        setup_game_fonts(ctx);
        setup_post_processing(ctx);

        // Set some parameters
        ctx->window->set_vsync(data.options.vsync);
        ctx->snd->get_listener().set_gain(data.options.master_volume);
        sm::music::set_music_gain(data.options.music_volume);

        // Setup scene framebuffer
        ctx->r3d->set_scene_framebuffer(data.launcher_options.samples);

        // Setup depth map framebuffer
        ctx->r3d->set_shadow_map_framebuffer(
            data.launcher_options.texture_quality == launcher_options::NORMAL ? 4096 : 2048
        );
    }

    void stop(sm::Ctx* ctx) {
        delete static_cast<Data*>(ctx->user_data);
    }
}
