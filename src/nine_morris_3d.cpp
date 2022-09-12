#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "nine_morris_3d.h"
#include "game/assets.h"
#include "game/assets_data.h"
#include "game/options.h"
#include "post_processing/bright_filter.h"
#include "post_processing/blur.h"
#include "post_processing/combine.h"

NineMorris3D::NineMorris3D(const ApplicationBuilder& builder)
    : Application(builder) {
    srand(time(nullptr));

    try {
        options::load_options_from_file(options);
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());
        options::handle_options_file_not_open_error(app_data.application_name);
    } catch (const options::OptionsFileError& e) {
        REL_ERROR("{}", e.what());

        try {
            options::create_options_file();
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());
        }
    }

    window->set_vsync(options.vsync);

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
        specification.samples = options.samples;
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
    renderer->set_depth_map_framebuffer(2048 /*options.texture_quality == NORMAL ? 4096 : 2048*/);  // FIXME this

    // Setup ImGui
    {
        ImGuiIO& io = ImGui::GetIO();

        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        builder.AddText(u8"ă");
        ImVector<ImWchar> ranges;
        builder.BuildRanges(&ranges);

        io.FontDefault = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 21.0f);
        imgui_info_font = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 16.0f);
        imgui_windows_font = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS_FONT).c_str(), 24.0f,
                nullptr, ranges.Data);
        io.Fonts->Build();
    }

    // Load splash screen
    {
        TextureSpecification specification;
        specification.min_filter = Filter::Linear;
        specification.mag_filter = Filter::Linear;

        res.textures.load("splash_screen_texture"_hs, encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
    }

    // Load and create this font
    {
        res.fonts.load("good_dog_plain_font"_hs, path_for_assets(GOOD_DOG_PLAIN_FONT), 50.0f, 5, 180, 40, 512);

        auto font = res.fonts["good_dog_plain_font"_hs];
        font->begin_baking();  // TODO maybe move part of texture baking to thread
        font->bake_characters(32, 127);
        font->end_baking();
    }

    // assets_data = std::make_shared<AssetsData>();  // FIXME this

    // Setup post-processing
    {
        FramebufferSpecification specification;
        specification.width = app_data.width / 2;
        specification.height = app_data.height / 2;
        specification.resize_divisor = 2;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
        };

        auto framebuffer = std::make_shared<Framebuffer>(specification);

        purge_framebuffers();
        add_framebuffer(framebuffer);

        auto shader = std::make_shared<Shader>(
            encr(path_for_assets(BRIGHT_FILTER_VERTEX_SHADER)),
            encr(path_for_assets(BRIGHT_FILTER_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );

        renderer->add_post_processing(std::make_shared<BrightFilter>("bright_filter", framebuffer, shader));
    }
    auto blur_shader = std::make_shared<Shader>(
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

        auto framebuffer = std::make_shared<Framebuffer>(specification);

        purge_framebuffers();
        add_framebuffer(framebuffer);

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

        auto framebuffer = std::make_shared<Framebuffer>(specification);

        purge_framebuffers();
        add_framebuffer(framebuffer);

        renderer->add_post_processing(std::make_shared<Blur>("blur2", framebuffer, blur_shader));
    }
    {
        FramebufferSpecification specification;
        specification.width = app_data.width;
        specification.height = app_data.height;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture)
        };

        auto framebuffer = std::make_shared<Framebuffer>(specification);

        auto shader = std::make_shared<Shader>(
            encr(path_for_assets(COMBINE_VERTEX_SHADER)),
            encr(path_for_assets(COMBINE_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture", "u_bright_texture", "u_strength" }
        );

        purge_framebuffers();
        add_framebuffer(framebuffer);

        auto combine = std::make_shared<Combine>("combine", framebuffer, shader);
        combine->strength = 0.7f;  // FIXME options.bloom_strength;
        renderer->add_post_processing(combine);
    }
}

NineMorris3D::~NineMorris3D() {

}

// void NineMorris3D::set_bloom(bool enable) {
//     const PostProcessingContext& context = renderer->get_post_processing_context();

//     for (size_t i = 0; i < context.steps.size(); i++) {
//         PostProcessingStep* step = context.steps[i].get();
//         step->enabled = enable;
//     }
// }

// void NineMorris3D::set_bloom_strength(float strength) {
//     const PostProcessingContext& context = renderer->get_post_processing_context();

//     auto iter = std::find_if(context.steps.begin(), context.steps.end(),
//             [](const std::shared_ptr<PostProcessingStep>& step) {
//         return step->get_name() == "combine";
//     });

//     ASSERT(iter != context.steps.end(), "Combine step must exist");

//     (*static_cast<Combine*>(iter->get())).strength = strength;
//     (*iter)->prepare(context);
// }
