#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/assets.h"
#include "game/game_options.h"
#include "game/post_processing/bright_filter.h"
#include "game/post_processing/blur.h"
#include "game/post_processing/combine.h"
#include "launcher/launcher_options.h"
#include "options/options.h"
#include "other/data.h"

namespace game {
    void start(Application* app) {
        srand(time(nullptr));

        auto& data = app->user_data<Data>();

        try {
            options::load_options_from_file<game_options::GameOptions>(
                data.options, game_options::GAME_OPTIONS_FILE, game_options::validate
            );
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());

            options::handle_options_file_not_open_error<game_options::GameOptions>(
                game_options::GAME_OPTIONS_FILE, app->data().application_name
            );
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());

            try {
                options::create_options_file<game_options::GameOptions>(game_options::GAME_OPTIONS_FILE);
            } catch (const options::OptionsFileNotOpenError& e) {
                REL_ERROR("{}", e.what());
            } catch (const options::OptionsFileError& e) {
                REL_ERROR("{}", e.what());
            }
        }

        app->window->set_vsync(data.options.vsync);

        using namespace assets;
        using namespace encrypt;
        using namespace paths;

        // Load and set icons
        {
            std::array<std::unique_ptr<TextureData>, 5> icons = {
                std::make_unique<TextureData>(path_for_assets(ICON_512), false),
                std::make_unique<TextureData>(path_for_assets(ICON_256), false),
                std::make_unique<TextureData>(path_for_assets(ICON_128), false),
                std::make_unique<TextureData>(path_for_assets(ICON_64), false),
                std::make_unique<TextureData>(path_for_assets(ICON_32), false)
            };

            app->window->set_icons<5>(icons);
        }

        // Load and set cursors
        data.arrow_cursor = app->window->add_cursor(
            std::make_unique<TextureData>(encr(path_for_assets(ARROW_CURSOR)), false),
            4, 1
        );
        data.cross_cursor = app->window->add_cursor(
            std::make_unique<TextureData>(encr(path_for_assets(CROSS_CURSOR)), false),
            8, 8
        );

        // Setup scene framebuffer
        {
            FramebufferSpecification specification;
            specification.width = app->data().width;
            specification.height = app->data().height;
            specification.samples = data.launcher_options.samples;
            specification.color_attachments = {
                Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture),
                Attachment(AttachmentFormat::RED_I, AttachmentType::Texture)
            };
            specification.depth_attachment = Attachment(
                AttachmentFormat::DEPTH24_STENCIL8, AttachmentType::Renderbuffer
            );
            constexpr int color[4] = { 0, 0, 0, 0 };
            specification.clear_drawbuffer = 1;
            specification.clear_value = color;

            auto framebuffer = app->res.framebuffer.load("scene_framebuffer"_h, specification);

            app->renderer->set_scene_framebuffer(framebuffer);
        }

        // Setup depth map framebuffer
        app->renderer->set_shadow_map_framebuffer(data.launcher_options.texture_quality == launcher_options::NORMAL ? 4096 : 2048);

        // Setup ImGui fonts
        {
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

        // Load and create this font
        {
            auto font = app->res.font.load("good_dog_plain_font"_h, path_for_assets(GOOD_DOG_PLAIN_FONT), 50.0f, 5, 180, 40, 512);

            font->begin_baking();  // TODO maybe move part of texture baking to thread
            font->bake_characters(32, 127);
            font->end_baking();
        }

        // Setup post-processing  // FIXME this is optional
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

    void stop(Application*) {

    }
}
