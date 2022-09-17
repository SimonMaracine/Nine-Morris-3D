#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "launcher/launcher_scene.h"
#include "game/assets.h"

#define DEFAULT_BROWN ImVec4(0.647f, 0.4f, 0.212f, 1.0f)
#define DARK_BROWN ImVec4(0.4f, 0.25f, 0.1f, 1.0f)
#define LIGHT_BROWN ImVec4(0.68f, 0.48f, 0.22f, 1.0f)
#define BEIGE ImVec4(0.961f, 0.875f, 0.733f, 1.0f)
#define LIGHT_GRAY_BLUE ImVec4(0.357f, 0.408f, 0.525f, 1.0f)

#define BACKGROUND ImVec4(0.058f, 0.058f, 0.058f, 0.745f)

void LauncherScene::on_start() {
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;

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
    colors[ImGuiCol_WindowBg] = BACKGROUND;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.FrameRounding = 8;
    style.WindowRounding = 8;
    style.ChildRounding = 8;
    style.PopupRounding = 8;
    style.GrabRounding = 8;

    app->evt.sink<WindowClosedEvent>().connect<&LauncherScene::on_window_closed>(*this);

    using namespace assets;
    using namespace encrypt;
    using namespace paths;

    // Load splash screen
    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    app->res.textures.load("splash_screen_texture"_hs, encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
}

void LauncherScene::on_stop() {
    
}

void LauncherScene::on_imgui_update() {
    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    app->gui_renderer->im_draw_quad(
        glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->res.textures["splash_screen_texture"_hs]
    );

    ImGui::ShowDemoWindow();
}

void LauncherScene::on_window_closed(const WindowClosedEvent&) {
    app->exit_code = 1;
}
