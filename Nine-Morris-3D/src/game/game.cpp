#include <engine/engine_application.h>
#include <engine/engine_graphics.h>
#include <engine/engine_audio.h>
#include <engine/engine_other.h>

#include "game/assets.h"
#include "game/game_options.h"
#include "game/post_processing/bright_filter.h"
#include "game/post_processing/blur.h"
#include "game/post_processing/combine.h"
#include "launcher/launcher_options.h"
#include "other/options.h"
#include "other/data.h"
#include "other/options_gracefully.h"

static void load_game_options(Application* app) {
    auto& data = app->user_data<Data>();

    options_gracefully::load_from_file<game_options::GameOptions>(
        game_options::GAME_OPTIONS_FILE, data.options, game_options::validate
    );
}

static void setup_icons(Application* app) {
    using namespace assets;
    using namespace file_system;

    const auto icons = {
        std::make_unique<TextureData>(path_for_assets(ICON_512), false),
        std::make_unique<TextureData>(path_for_assets(ICON_256), false),
        std::make_unique<TextureData>(path_for_assets(ICON_128), false),
        std::make_unique<TextureData>(path_for_assets(ICON_64), false),
        std::make_unique<TextureData>(path_for_assets(ICON_32), false)
    };

    app->window->set_icons(icons);
}

static void setup_cursors(Application* app) {
    auto& data = app->user_data<Data>();

    using namespace assets;
    using namespace file_system;
    using namespace encrypt;

    data.arrow_cursor = app->window->add_cursor(
        std::make_unique<TextureData>(encr(path_for_assets(ARROW_CURSOR)), false),
        4, 1
    );
    data.cross_cursor = app->window->add_cursor(
        std::make_unique<TextureData>(encr(path_for_assets(CROSS_CURSOR)), false),
        8, 8
    );
}

static void setup_imgui_fonts(Application* app) {
    auto& data = app->user_data<Data>();

    using namespace assets;
    using namespace file_system;

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

static void setup_game_fonts(Application* app) {
    using namespace assets;
    using namespace file_system;

    {
        auto font = app->res.font.load(
            "good_dog_plain"_h, path_for_assets(GOOD_DOG_PLAIN_FONT), 50.0f, 5, 180, 40, 512
        );

        font->begin_baking();
        font->bake_characters(32, 127);
        font->end_baking();
    }

    {
        auto font = app->res.font.load(
            "open_sans"_h, path_for_assets(OPEN_SANS_SEMIBOLD_FONT), 50.0f, 5, 180, 40, 512
        );

        font->begin_baking();
        font->bake_characters(32, 127);
        font->end_baking();
    }
}

static void setup_post_processing(Application* app) {
    auto& data = app->user_data<Data>();

    if (!data.launcher_options.bloom) {
        return;  // Disable bloom
    }

    using namespace assets;
    using namespace file_system;
    using namespace encrypt;

    {
        gl::FramebufferSpecification specification;
        specification.width = app->data().width / 2;
        specification.height = app->data().height / 2;
        specification.resize_divisor = 2;
        specification.color_attachments = {
            gl::Attachment {gl::AttachmentFormat::RGBA8, gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<gl::Framebuffer>(specification);

        app->purge_framebuffers();
        app->add_framebuffer(framebuffer);

        auto shader = std::make_shared<gl::Shader>(
            encr(path_for_assets(BRIGHT_FILTER_VERTEX_SHADER)),
            encr(path_for_assets(BRIGHT_FILTER_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );

        app->renderer->add_post_processing(std::make_unique<BrightFilter>("bright_filter", framebuffer, shader));
    }

    auto blur_shader = std::make_shared<gl::Shader>(
        encr(path_for_assets(BLUR_VERTEX_SHADER)),
        encr(path_for_assets(BLUR_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_screen_texture" }
    );

    {
        gl::FramebufferSpecification specification;
        specification.width = app->data().width / 4;
        specification.height = app->data().height / 4;
        specification.resize_divisor = 4;
        specification.color_attachments = {
            gl::Attachment {gl::AttachmentFormat::RGBA8, gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<gl::Framebuffer>(specification);

        app->purge_framebuffers();
        app->add_framebuffer(framebuffer);

        app->renderer->add_post_processing(std::make_unique<Blur>("blur1", framebuffer, blur_shader));
    }
    {
        gl::FramebufferSpecification specification;
        specification.width = app->data().width / 8;
        specification.height = app->data().height / 8;
        specification.resize_divisor = 8;
        specification.color_attachments = {
            gl::Attachment {gl::AttachmentFormat::RGBA8, gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<gl::Framebuffer>(specification);

        app->purge_framebuffers();
        app->add_framebuffer(framebuffer);

        app->renderer->add_post_processing(std::make_unique<Blur>("blur2", framebuffer, blur_shader));
    }
    {
        gl::FramebufferSpecification specification;
        specification.width = app->data().width;
        specification.height = app->data().height;
        specification.color_attachments = {
            gl::Attachment {gl::AttachmentFormat::RGBA8, gl::AttachmentType::Texture}
        };

        auto framebuffer = std::make_shared<gl::Framebuffer>(specification);

        auto shader = std::make_shared<gl::Shader>(
            encr(path_for_assets(COMBINE_VERTEX_SHADER)),
            encr(path_for_assets(COMBINE_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture", "u_bright_texture", "u_strength" }
        );

        app->purge_framebuffers();
        app->add_framebuffer(framebuffer);

        auto combine = std::make_unique<Combine>("combine", framebuffer, shader);
        combine->strength = data.launcher_options.bloom_strength;
        app->renderer->add_post_processing(std::move(combine));
    }
}

namespace game {
    void start(Application* app) {
        auto& data = app->user_data<Data>();

        load_game_options(app);
        setup_icons(app);
        setup_cursors(app);
        setup_imgui_fonts(app);
        setup_game_fonts(app);
        setup_post_processing(app);

        // Set some parameters
        app->window->set_vsync(data.options.vsync);
        app->openal->get_listener().set_gain(data.options.master_volume);
        music::set_music_gain(data.options.music_volume);

        // Setup scene framebuffer
        app->renderer->set_scene_framebuffer(data.launcher_options.samples);

        // Setup depth map framebuffer
        app->renderer->set_shadow_map_framebuffer(
            data.launcher_options.texture_quality == launcher_options::NORMAL ? 4096 : 2048
        );
    }

    void stop(Application* app) {
        app->destroy_user_data();
    }
}
