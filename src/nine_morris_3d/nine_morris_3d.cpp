#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "application/platform.h"
#include "graphics/renderer/buffer_layout.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/buffer.h"
#include "graphics/renderer/opengl/texture.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/assets.h"
#include "nine_morris_3d/assets_data.h"
#include "other/paths.h"
#include "other/texture_data.h"
#include "other/logging.h"
#include "other/assert.h"
#include "other/encryption.h"

// Global reference to application
NineMorris3D* app = nullptr;

NineMorris3D::NineMorris3D(std::string_view info_file, std::string_view log_file, std::string_view application_name)
    : Application(1024, 576, "Nine Morris 3D", info_file, log_file, application_name) {
    try {
        options::load_options_from_file(options);
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());
        options::handle_options_file_not_open_error();
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

    srand(time(nullptr));

    assets::maybe_initialize_assets();

    using namespace assets;
    using namespace encryption;
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
    arrow_cursor = window->add_cursor(std::make_unique<TextureData>(convert(path_for_assets(ARROW_CURSOR)), false), 4, 1);
    cross_cursor = window->add_cursor(std::make_unique<TextureData>(convert(path_for_assets(CROSS_CURSOR)), false), 8, 8);

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

        renderer->set_scene_framebuffer(Framebuffer::create(specification));
    }

    // Setup depth map framebuffer
    renderer->set_depth_map_framebuffer(options.texture_quality == options::NORMAL ? 4096 : 2048);

    // Initialize and setup ImGui
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 430 core");
    ImGui_ImplGlfw_InitForOpenGL(window->get_handle(), false);

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

    // Load splash screen and keyboard controls
    data.splash_screen_texture = Texture::create(convert(path_for_assets(SPLASH_SCREEN_TEXTURE)), true);
    data.keyboard_controls_texture = Texture::create(convert(path_for_assets(KEYBOARD_CONTROLS_TEXTURE)), true);

    // Load and create this font
    data.good_dog_plain_font = std::make_shared<Font>(path_for_assets(GOOD_DOG_PLAIN_FONT), 50.0f, 5, 180, 40, 512);
    data.good_dog_plain_font->begin_baking();  // TODO maybe move part of texture baking to thread
    data.good_dog_plain_font->bake_characters(32, 127);
    data.good_dog_plain_font->end_baking();

    assets_data = std::make_shared<AssetsData>();
}

NineMorris3D::~NineMorris3D() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void NineMorris3D::set_app_pointer(NineMorris3D* instance) {
    ASSERT(app == nullptr, "App cannot be set twice");
    app = instance;
}
