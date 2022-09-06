#include <imgui.h>
#include <entt/entt.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "nine_morris_3d.h"
#include "assets.h"
#include "assets_data.h"
#include "game/components/options.h"
#include "game/systems/options.h"
#include "post_processing/bright_filter.h"
#include "post_processing/blur.h"
#include "post_processing/combine.h"

NineMorris3D::NineMorris3D(const ApplicationBuilder& builder)
    : Application(builder) {
    // try {
    //     options::load_options_from_file(options);
    // } catch (const options::OptionsFileNotOpenError& e) {
    //     REL_ERROR("{}", e.what());
    //     options::handle_options_file_not_open_error();
    // } catch (const options::OptionsFileError& e) {
    //     REL_ERROR("{}", e.what());

    //     try {
    //         options::create_options_file();
    //     } catch (const options::OptionsFileNotOpenError& e) {
    //         REL_ERROR("{}", e.what());
    //     } catch (const options::OptionsFileError& e) {
    //         REL_ERROR("{}", e.what());
    //     }
    // }

    // TODO this is the new one
    // auto options = registry.create();
    // registry.emplace<OptionsComponent>(options);

    // try {
    //     load_options_system(registry);
    // } catch (const OptionsFileNotOpenError& e) {
    //     REL_ERROR("{}", e.what());
    //     handle_options_file_not_open_error();
    // } catch (const OptionsFileError& e) {
    //     REL_ERROR("{}", e.what());

    //     try {
    //         create_options_file();
    //     } catch (const OptionsFileNotOpenError& e) {
    //         REL_ERROR("{}", e.what());
    //     } catch (const OptionsFileError& e) {
    //         REL_ERROR("{}", e.what());
    //     }
    // }

    // auto& options_c = registry.get<OptionsComponent>(options);

    // window->set_vsync(options_c.vsync);

    srand(time(nullptr));

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

        window->set_icons<5>(icons);
    }

    // Load and set cursors
    arrow_cursor = window->add_cursor(std::make_unique<TextureData>(encr(path_for_assets(ARROW_CURSOR)), false), 4, 1);
    cross_cursor = window->add_cursor(std::make_unique<TextureData>(encr(path_for_assets(CROSS_CURSOR)), false), 8, 8);

    // Setup scene framebuffer
    {
        FramebufferSpecification specification;
        specification.width = app_data.width;
        specification.height = app_data.height;
        specification.samples = 2;  // FIXME options_c.samples;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture),
            Attachment(AttachmentFormat::RED_I, AttachmentType::Texture)
        };
        specification.depth_attachment = Attachment(
            AttachmentFormat::DEPTH24_STENCIL8, AttachmentType::Renderbuffer
        );

        renderer->set_scene_framebuffer(std::make_shared<Framebuffer>(specification));
    }

    // Setup depth map framebuffer
    renderer->set_depth_map_framebuffer(2048 /*FIXME options_c.texture_quality == NORMAL ? 4096 : 2048*/);

    // Setup ImGui
    {
        ImGuiIO& io = ImGui::GetIO();

        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        builder.AddText(u8"ă");
        ImVector<ImWchar> ranges;
        builder.BuildRanges(&ranges);

        io.FontDefault = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 21.0f);
        // data.imgui_info_font = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 16.0f);
        // data.imgui_windows_font = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 24.0f,
                // nullptr, ranges.Data);
        io.Fonts->Build();
    }

    // Load splash screen
    {
        TextureSpecification specification;
        specification.min_filter = Filter::Linear;
        specification.mag_filter = Filter::Linear;

        // data.splash_screen_texture = Texture::create(encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
    }

    // Load and create this font
    {
        // data.good_dog_plain_font = std::make_shared<Font>(path_for_assets(GOOD_DOG_PLAIN_FONT), 50.0f, 5, 180, 40, 512);
        // data.good_dog_plain_font->begin_baking();  // TODO maybe move part of texture baking to thread
        // data.good_dog_plain_font->bake_characters(32, 127);
        // data.good_dog_plain_font->end_baking();
    }

    // assets_data = std::make_shared<AssetsData>();

    // Setup post-processing
    {
        FramebufferSpecification specification;
        specification.width = app_data.width / 2;
        specification.height = app_data.height / 2;
        specification.resize_divisor = 2;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
        };

        std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>(specification);

        renderer->purge_framebuffers();
        renderer->add_framebuffer(framebuffer);

        std::shared_ptr<Shader> shader = std::make_shared<Shader>(
            encr(path_for_assets(BRIGHT_FILTER_VERTEX_SHADER)),
            encr(path_for_assets(BRIGHT_FILTER_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );

        renderer->add_post_processing(std::make_shared<BrightFilter>("bright_filter", framebuffer, shader));
    }
    std::shared_ptr<Shader> blur_shader = std::make_shared<Shader>(
        encr(path_for_assets(BLUR_VERTEX_SHADER)),
        encr(path_for_assets(BLUR_FRAGMENT_SHADER)),
        std::vector<std::string> { "u_screen_texture" }
    );
    {
        FramebufferSpecification specification;
        specification.width = app_data.width / 4;
        specification.height = app_data.height / 4;
        specification.resize_divisor = 4;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
        };

        std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>(specification);

        renderer->purge_framebuffers();
        renderer->add_framebuffer(framebuffer);

        renderer->add_post_processing(std::make_shared<Blur>("blur1", framebuffer, blur_shader));
    }
    {
        FramebufferSpecification specification;
        specification.width = app_data.width / 8;
        specification.height = app_data.height / 8;
        specification.resize_divisor = 8;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
        };

        std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>(specification);

        renderer->purge_framebuffers();
        renderer->add_framebuffer(framebuffer);

        renderer->add_post_processing(std::make_shared<Blur>("blur2", framebuffer, blur_shader));
    }
    {
        FramebufferSpecification specification;
        specification.width = app_data.width;
        specification.height = app_data.height;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
        };

        std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>(specification);

        std::shared_ptr<Shader> shader = std::make_shared<Shader>(
            encr(path_for_assets(COMBINE_VERTEX_SHADER)),
            encr(path_for_assets(COMBINE_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture", "u_bright_texture", "u_strength" }
        );

        renderer->purge_framebuffers();
        renderer->add_framebuffer(framebuffer);

        std::shared_ptr<Combine> combine = std::make_shared<Combine>("combine", framebuffer, shader);
        combine->strength = 0.7f;  // FIXME options_c.bloom_strength;
        renderer->add_post_processing(combine);
    }
}

NineMorris3D::~NineMorris3D() {

}

void NineMorris3D::set_bloom(bool enable) {
    const PostProcessingContext& context = renderer->get_post_processing_context();

    for (size_t i = 0; i < context.steps.size(); i++) {
        PostProcessingStep* step = context.steps[i].get();
        step->enabled = enable;
    }
}

void NineMorris3D::set_bloom_strength(float strength) {
    const PostProcessingContext& context = renderer->get_post_processing_context();

    auto iter = std::find_if(context.steps.begin(), context.steps.end(), [](std::shared_ptr<PostProcessingStep> step) {
        return step->get_id() == "combine";
    });

    ASSERT(iter != context.steps.end(), "Combine step must exist");

    (*static_cast<Combine*>(iter->get())).strength = strength;
    (*iter)->prepare(context);
}
