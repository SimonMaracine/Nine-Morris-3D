#include <vector>
#include <memory>
#include <string>
#include <stdlib.h>
#include <time.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "application/icon_image.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/buffer_layout.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/framebuffer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/assets.h"
#include "other/logging.h"

// Global reference to application
NineMorris3D* app = nullptr;

NineMorris3D::NineMorris3D()
    : Application(1024, 576, "Nine Morris 3D") {
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

    using namespace assets;

    // Load and set icons
    const std::array<std::unique_ptr<IconImage>, 5> icons = {
        std::make_unique<IconImage>(path(ICON_512)),
        std::make_unique<IconImage>(path(ICON_256)),
        std::make_unique<IconImage>(path(ICON_128)),
        std::make_unique<IconImage>(path(ICON_64)),
        std::make_unique<IconImage>(path(ICON_32))
    };

    window->set_icons<5>(icons);

    // Load and set cursors
    arrow_cursor = window->add_cursor(std::make_unique<IconImage>(path(ARROW_CURSOR)), 4, 1);
    cross_cursor = window->add_cursor(std::make_unique<IconImage>(path(CROSS_CURSOR)), 8, 8);

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
        specification.depth_attachment = Attachment(AttachmentFormat::DEPTH24_STENCIL8,
                AttachmentType::Renderbuffer);

        renderer->set_scene_framebuffer(Framebuffer::create(specification));
    }

    // Setup ImGui
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#ifdef NINE_MORRIS_3D_RELEASE
    io.IniFilename = nullptr;
#endif

    ImGui_ImplOpenGL3_Init("#version 430 core");
    ImGui_ImplGlfw_InitForOpenGL(window->get_handle(), false);
}

NineMorris3D::~NineMorris3D() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void NineMorris3D::set_app_pointer(NineMorris3D* instance) {
    assert(app == nullptr);
    app = instance;
}
