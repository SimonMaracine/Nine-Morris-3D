#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "launcher/launcher_scene.h"
#include "imgui_style/colors.h"

static const char* SPLASH_SCREEN = ENCR("data/textures/splash_screen/launcher/launcher_splash_screen.png");
static const char* OPEN_SANS = "data/fonts/OpenSans/OpenSans-Semibold.ttf";

void LauncherScene::on_start() {
    using namespace encrypt;
    using namespace paths;

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoDecoration;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = DEFAULT_BROWN;
    colors[ImGuiCol_TitleBgActive] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBg] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBgHovered] = DARK_BROWN;
    colors[ImGuiCol_FrameBgActive] = LIGHT_BROWN;
    colors[ImGuiCol_Button] = DARK_BROWN;
    colors[ImGuiCol_ButtonHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_ButtonActive] = LIGHT_BROWN;
    colors[ImGuiCol_Header] = DARK_BROWN;
    colors[ImGuiCol_HeaderHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_HeaderActive] = LIGHT_BROWN;
    colors[ImGuiCol_CheckMark] = BEIGE;
    colors[ImGuiCol_SliderGrab] = LIGHT_GRAY_BLUE;
    colors[ImGuiCol_SliderGrabActive] = LIGHT_GRAY_BLUE;
    colors[ImGuiCol_Tab] = DARK_BROWN;
    colors[ImGuiCol_TabHovered] = LIGHT_BROWN;
    colors[ImGuiCol_TabActive] = DEFAULT_BROWN;
    colors[ImGuiCol_WindowBg] = TRANSPARENT_BACKGROUND;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.FrameRounding = 8;
    style.WindowRounding = 8;
    style.ChildRounding = 8;
    style.PopupRounding = 8;
    style.GrabRounding = 8;

    // Setup ImGui fonts
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"ă");
    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);

    io.FontDefault = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS).c_str(), 21.0f, nullptr, ranges.Data);
    io.Fonts->Build();

    // Load splash screen
    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    app->res.textures.load("splash_screen"_hs, encr(path_for_assets(SPLASH_SCREEN)), specification);

    app->evt.sink<WindowClosedEvent>().connect<&LauncherScene::on_window_closed>(*this);
}

void LauncherScene::on_stop() {
    
}

void LauncherScene::on_imgui_update() {
    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    app->gui_renderer->im_draw_quad(
        glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->res.textures["splash_screen"_hs]
    );

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Launcher", nullptr, window_flags);
    
    if (ImGui::BeginTabBar("Tabs")) {
        if (ImGui::BeginTabItem("Graphics")) {
            ImGui::Text("Hello, world!");
            ImGui::Text("Hello, Simon.");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    
    ImGui::End();

    ImGui::ShowDemoWindow();
}

void LauncherScene::on_window_closed(const WindowClosedEvent&) {
    app->exit_code = 1;
}
