#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/assets.h"
#include "game/game_options.h"
#include "game/post_processing/bright_filter.h"
#include "game/post_processing/blur.h"
#include "game/post_processing/combine.h"
#include "launcher/launcher_options.h"
#include "other/options.h"
#include "other/data.h"
#include "other/save_load_gracefully.h"

namespace game {
    static void load_game_options(Application* app) {
        auto& data = app->user_data<Data>();

        save_load_gracefully::load_from_file<game_options::GameOptions>(
            game_options::GAME_OPTIONS_FILE, data.options, game_options::validate, app
        );
    }

    static void setup_icons(Application* app) {
        using namespace assets;
        using namespace paths;

        const std::array<std::unique_ptr<TextureData>, 5> icons = {
            std::make_unique<TextureData>(path_for_assets(ICON_512), false),
            std::make_unique<TextureData>(path_for_assets(ICON_256), false),
            std::make_unique<TextureData>(path_for_assets(ICON_128), false),
            std::make_unique<TextureData>(path_for_assets(ICON_64), false),
            std::make_unique<TextureData>(path_for_assets(ICON_32), false)
        };

        app->window->set_icons<5>(icons);
    }

    static void setup_cursors(Application* app) {
        auto& data = app->user_data<Data>();

        using namespace assets;
        using namespace paths;
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
        using namespace paths;

        ImGuiIO& io = ImGui::GetIO();

        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        builder.AddText(u8"ă");
        ImVector<ImWchar> ranges;
        builder.BuildRanges(&ranges);

        io.FontDefault = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 21.0f);
        data.imgui_info_font = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 16.0f);
        data.imgui_windows_font = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 24.0f,
                nullptr, ranges.Data);
        io.Fonts->Build();
    }

    static void setup_game_font(Application* app) {
        using namespace assets;
        using namespace paths;

        auto font = app->res.font.load(
            "good_dog_plain_font"_h, path_for_assets(GOOD_DOG_PLAIN_FONT), 50.0f, 5, 180, 40, 512
        );

        font->begin_baking();  // TODO maybe move part of texture baking to thread
        font->bake_characters(32, 127);
        font->end_baking();
    }

    static void setup_post_processing(Application* app) {
        auto& data = app->user_data<Data>();

        using namespace assets;
        using namespace paths;
        using namespace encrypt;

        {
            FramebufferSpecification specification;
            specification.width = app->data().width / 2;
            specification.height = app->data().height / 2;
            specification.resize_divisor = 2;
            specification.color_attachments = {
                Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
            };

            auto framebuffer = std::make_shared<Framebuffer>(specification);

            app->purge_framebuffers();
            app->add_framebuffer(framebuffer);

            auto shader = std::make_shared<Shader>(
                encr(path_for_assets(BRIGHT_FILTER_VERTEX_SHADER)),
                encr(path_for_assets(BRIGHT_FILTER_FRAGMENT_SHADER)),
                std::vector<std::string> { "u_screen_texture" }
            );

            app->renderer->add_post_processing(std::make_shared<BrightFilter>("bright_filter", framebuffer, shader));
        }

        auto blur_shader = std::make_shared<Shader>(
            encr(path_for_assets(BLUR_VERTEX_SHADER)),
            encr(path_for_assets(BLUR_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );

        {
            FramebufferSpecification specification;
            specification.width = app->data().width / 4;
            specification.height = app->data().height / 4;
            specification.resize_divisor = 4;
            specification.color_attachments = {
                Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
            };

            auto framebuffer = std::make_shared<Framebuffer>(specification);

            app->purge_framebuffers();
            app->add_framebuffer(framebuffer);

            app->renderer->add_post_processing(std::make_shared<Blur>("blur1", framebuffer, blur_shader));
        }
        {
            FramebufferSpecification specification;
            specification.width = app->data().width / 8;
            specification.height = app->data().height / 8;
            specification.resize_divisor = 8;
            specification.color_attachments = {
                Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
            };

            auto framebuffer = std::make_shared<Framebuffer>(specification);

            app->purge_framebuffers();
            app->add_framebuffer(framebuffer);

            app->renderer->add_post_processing(std::make_shared<Blur>("blur2", framebuffer, blur_shader));
        }
        {
            FramebufferSpecification specification;
            specification.width = app->data().width;
            specification.height = app->data().height;
            specification.color_attachments = {
                Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
            };

            auto framebuffer = std::make_shared<Framebuffer>(specification);

            auto shader = std::make_shared<Shader>(
                encr(path_for_assets(COMBINE_VERTEX_SHADER)),
                encr(path_for_assets(COMBINE_FRAGMENT_SHADER)),
                std::vector<std::string> { "u_screen_texture", "u_bright_texture", "u_strength" }
            );

            app->purge_framebuffers();
            app->add_framebuffer(framebuffer);

            auto combine = std::make_shared<Combine>("combine", framebuffer, shader);
            combine->strength = data.launcher_options.bloom_strength;
            app->renderer->add_post_processing(combine);
        }
    }

    void start(Application* app) {
        auto& data = app->user_data<Data>();

        srand(time(nullptr));

        load_game_options(app);
        setup_icons(app);
        setup_cursors(app);
        setup_imgui_fonts(app);
        setup_game_font(app);
        setup_post_processing(app);  // FIXME this is optional

        app->window->set_vsync(data.options.vsync);

        // Setup scene framebuffer
        app->renderer->set_scene_framebuffer(data.launcher_options.samples);

        // Setup depth map framebuffer
        app->renderer->set_shadow_map_framebuffer(
            data.launcher_options.texture_quality == launcher_options::NORMAL ? 4096 : 2048
        );
    }

    void stop(Application*) {

    }
}
