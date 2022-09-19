#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "launcher/launcher_scene.h"
#include "launcher/launcher_options.h"
#include "game/game.h"
#include "options/options.h"
#include "imgui_style/colors.h"

static const char* SPLASH_SCREEN = ENCR("data/textures/splash_screen/launcher/launcher_splash_screen.png");
static const char* OPEN_SANS = "data/fonts/OpenSans/OpenSans-Semibold.ttf";

// TODO rework options system a bit

constexpr size_t RESOLUTIONS_COUNT = 5;
static const char* resolutions[RESOLUTIONS_COUNT] = { "512x288", "768x432", "1024x576", "1280x720", "1536x864" };

constexpr size_t TEXTURE_QUALITIES_COUNT = 2;
static const char* texture_qualities[TEXTURE_QUALITIES_COUNT] = { "Normal", "Low" };

static size_t map_resolution_to_index(const std::tuple<int, int>& resolution) {
    switch (std::get<0>(resolution)) {
        case 512:
            return 0;
        case 768:
            return 1;
        case 1024:
            return 2;
        case 1280:
            return 3;            
        case 1536:
            return 4;
    }

    return 2;  // Default
}

static std::tuple<int, int> map_index_to_resolution(size_t index) {
    switch (index) {
        case 0:
            return {512, 288};
        case 1:
            return {768, 432};
        case 2:
            return {1024, 576};
        case 3:
            return {1280, 720};
        case 4:
            return {1536, 864};
    }

    return {1024, 576};  // Default
}

static size_t map_texture_quality_to_index(std::string_view texture_quality) {
    if (strcmp(texture_quality.data(), launcher_options::NORMAL) == 0) {
        return 0;
    } else if (strcmp(texture_quality.data(), launcher_options::LOW) == 0) {
        return 1;
    }

    return 0;  // Default
}

static std::string map_index_to_texture_quality(size_t index) {
    switch (index) {
        case 0:
            return launcher_options::NORMAL;
        case 1:
            return launcher_options::LOW;
    }

    return launcher_options::NORMAL;  // Default
}

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

    // Load options
    using namespace launcher_options;

    auto& data = app->user_data<game::Data>();

    try {
        options::load_options_from_file<LauncherOptions>(
            data.launcher_options, LAUNCHER_OPTIONS_FILE, validate
        );
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());

        options::handle_options_file_not_open_error<LauncherOptions>(
            LAUNCHER_OPTIONS_FILE, app->data().application_name
        );
    } catch (const options::OptionsFileError& e) {
        REL_ERROR("{}", e.what());

        try {
            options::create_options_file<LauncherOptions>(LAUNCHER_OPTIONS_FILE);
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());
        }
    }

    // Set events
    app->evt.sink<WindowClosedEvent>().connect<&LauncherScene::on_window_closed>(*this);
}

void LauncherScene::on_stop() {
    // Save options
    using namespace launcher_options;

    auto& data = app->user_data<game::Data>();

    try {
        options::save_options_to_file<LauncherOptions>(data.launcher_options, LAUNCHER_OPTIONS_FILE);
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());

        options::handle_options_file_not_open_error<LauncherOptions>(
            LAUNCHER_OPTIONS_FILE, app->data().application_name
        );
    } catch (const options::OptionsFileError& e) {
        REL_ERROR("{}", e.what());

        try {
            options::create_options_file<LauncherOptions>(LAUNCHER_OPTIONS_FILE);
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());
        }
    }
}

void LauncherScene::on_imgui_update() {
    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    app->gui_renderer->im_draw_quad(
        glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->res.textures["splash_screen"_hs]
    );

    auto& data = app->user_data<game::Data>();

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Launcher", nullptr, window_flags);

    if (ImGui::BeginTabBar("Tabs")) {
        if (ImGui::BeginTabItem("Welcome")) {
            // FIXME add proper content
            ImGui::Text("Nine Morris 3D: a nice board game");
            ImGui::Text("Version %u.%u.%u", app->data().version_major, app->data().version_minor, app->data().version_patch);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Graphics")) {
            ImGui::PushItemWidth(150.0f);

            ImGui::Text("Fullscreen"); ImGui::SameLine();
            ImGui::Checkbox("##Fullscreen", &data.launcher_options.fullscreen);

            ImGui::Text("Resolution"); ImGui::SameLine();
            static size_t resolution_index = map_resolution_to_index(data.launcher_options.resolution);
            const char* combo_preview_value = resolutions[resolution_index];
            if (ImGui::BeginCombo("##Resolution", combo_preview_value)) {
                for (size_t i = 0; i < RESOLUTIONS_COUNT; i++) {
                    const bool is_selected = resolution_index == i;

                    if (ImGui::Selectable(resolutions[i], is_selected)) {
                        resolution_index = i;
                        data.launcher_options.resolution = map_index_to_resolution(i);
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::Text("Texture Quality"); ImGui::SameLine();
            static size_t texture_quality_index = map_texture_quality_to_index(data.launcher_options.texture_quality);
            combo_preview_value = texture_qualities[texture_quality_index];
            if (ImGui::BeginCombo("##Texture Quality", combo_preview_value)) {
                for (size_t i = 0; i < 2; i++) {
                    const bool is_selected = texture_quality_index == i;

                    if (ImGui::Selectable(texture_qualities[i], is_selected)) {
                        texture_quality_index = i;
                        data.launcher_options.texture_quality = map_index_to_texture_quality(i);
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::Text("Normal Mapping"); ImGui::SameLine();
            ImGui::Checkbox("##Normal Mapping", &data.launcher_options.normal_mapping);

            ImGui::Text("Bloom"); ImGui::SameLine();
            ImGui::Checkbox("##Bloom", &data.launcher_options.bloom);

            ImGui::Text("Bloom Strength"); ImGui::SameLine();
            ImGui::SliderFloat(
                "##Bloom Strength", 
                &data.launcher_options.bloom_strength,
                0.1f, 1.0f, "%.01f",
                ImGuiSliderFlags_Logarithmic
            );

            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Dummy(ImVec2(60.0f, 0.0f)); ImGui::SameLine();

    if (ImGui::Button("Play", ImVec2(120.0f, 32.0f))) {
        app->exit_code = 0;
        app->running = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Quit", ImVec2(120.0f, 32.0f))) {
        app->exit_code = 1;
        app->running = false;
    }

    ImGui::End();

    ImGui::ShowDemoWindow();
}

void LauncherScene::on_window_closed(const WindowClosedEvent&) {
    app->exit_code = 1;
}
