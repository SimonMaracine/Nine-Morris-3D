#include <engine/public/other.h>
#include <engine/public/external/imgui.h++>
#include <engine/public/external/resmanager.h++>

#include "launcher/scenes/launcher_scene.h"
#include "launcher/launcher_options.h"
#include "other/options.h"
#include "other/data.h"
#include "other/constants.h"
#include "other/options_gracefully.h"

static const char* SPLASH_SCREEN = ENCR("data/textures/splash_screen/launcher/launcher_splash_screen.png");
static const char* OPEN_SANS = "data/fonts/OpenSans/OpenSans-Semibold.ttf";

static const char* RESOLUTIONS[] = {
    "512x288", "768x432", "1024x576", "1280x720", "1536x864", "1792x1008"  // TODO this is a mess
};

// There seems to be no better way

static size_t map_resolution_to_index(const std::pair<int, int>& resolution) {
    static constexpr size_t DEFAULT = 2;

    switch (resolution.first) {
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
        case 1792:
            return 5;
    }

    LOG_DIST_ERROR("Using default index for resolution");

    return DEFAULT;
}

static std::pair<int, int> map_index_to_resolution(size_t index) {
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
        case 5:
            return {1792, 1008};
    }

    LOG_DIST_ERROR("Using default resolution for index");

    return launcher_options::DEFAULT_RESOLUTION;
}

static size_t map_texture_quality_to_index(int texture_quality) {
    static constexpr size_t DEFAULT = 0;

    if (texture_quality == launcher_options::NORMAL) {
        return 0;
    } else if (texture_quality == launcher_options::LOW) {
        return 1;
    }

    LOG_DIST_ERROR("Using default index for texture_quality");

    return DEFAULT;
}

static int map_index_to_texture_quality(size_t index) {
    switch (index) {
        case 0:
            return launcher_options::NORMAL;
        case 1:
            return launcher_options::LOW;
    }

    LOG_DIST_ERROR("Using default texture_quality for index");

    return launcher_options::DEFAULT_TEXTURE_QUALITY;
}

static size_t map_samples_to_index(int samples) {
    static constexpr size_t DEFAULT = 1;

    switch (samples) {
        case 1:
            return 0;
        case 2:
            return 1;
        case 4:
            return 2;
    }

    LOG_DIST_ERROR("Using default index for samples");

    return DEFAULT;
}

static int map_index_to_samples(size_t index) {
    switch (index) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 4;
    }

    LOG_DIST_ERROR("Using default samples for index");

    return launcher_options::DEFAULT_SAMPLES;
}

static size_t map_anisotropic_filtering_to_index(int anisotropic_filtering) {
    static constexpr size_t DEFAULT = 1;

    switch (anisotropic_filtering) {
        case 0:
            return 0;
        case 4:
            return 1;
        case 8:
            return 2;
    }

    LOG_DIST_ERROR("Using default index for anisotropic_filtering");

    return DEFAULT;
}

static int map_index_to_anisotropic_filtering(size_t index) {
    switch (index) {
        case 0:
            return 0;
        case 1:
            return 4;
        case 2:
            return 8;
    }

    LOG_DIST_ERROR("Using default anisotropic_filtering for index");

    return launcher_options::DEFAULT_ANISOTROPIC_FILTERING;
}

static std::vector<const char*> options_sample() {
    static const char* OPTIONS[] = {
        "Off", "2x", "4x"
    };

    std::vector<const char*> options;

    const int max = sm::max_samples_supported();

    size_t options_supported = 0;

    if (max < 2) {
        options_supported = 1;
    } else if (max < 4) {
        options_supported = 2;
    } else {
        options_supported = 3;
    }

    for (size_t i = 0; i < options_supported; i++) {
        options.push_back(OPTIONS[i]);
    }

    return options;
}

static std::vector<const char*> options_anisotropic_filtering() {
    static const char* OPTIONS[] = {
        "Off", "4x", "8x"
    };

    std::vector<const char*> options;

    const int max = sm::max_anisotropic_filtering_supported();

    size_t options_supported = 0;

    if (max < 4) {
        options_supported = 1;
    } else if (max < 8) {
        options_supported = 2;
    } else {
        options_supported = 3;
    }

    for (size_t i = 0; i < options_supported; i++) {
        options.push_back(OPTIONS[i]);
    }

    return options;
}

static void combo(const char* title, const char* id, size_t& index, const std::vector<const char*>& options,
        const std::function<void(size_t)>& action) {
    ImGui::Text("%s", title); ImGui::SameLine();

    index = std::min(index, options.size() - 1);
    action(index);

    const char* combo_preview_value = options[index];

    if (ImGui::BeginCombo(id, combo_preview_value)) {
        for (size_t i = 0; i < options.size(); i++) {
            const bool is_selected = index == i;

            if (ImGui::Selectable(options[i], is_selected)) {
                index = i;
                action(i);
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
}

static void help_marker(const char* text) {
    ImGui::TextDisabled("(?)");

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", text);
    }
}

void LauncherScene::on_start() {
    auto& data = ctx->data<Data>();

    using namespace sm::file_system;

    // Load splash screen
    ctx->res.texture.load("splash_screen"_H, sm::Encrypt::encr(path_for_assets(SPLASH_SCREEN)), sm::gl::TextureSpecification {});

    auto background = objects.add<sm::gui::Image>("background"_H, ctx->res.texture["splash_screen"_H]);
    scene_list.add(background);

    // Load launcher options from file
    options_gracefully::load_from_file<launcher_options::LauncherOptions>(
        launcher_options::LAUNCHER_OPTIONS_FILE, data.launcher_options, launcher_options::validate_load
    );

    // Calculate available screen resolutions
    initialize_resolutions();
}

void LauncherScene::on_stop() {
    auto& data = ctx->data<Data>();

    options_gracefully::save_to_file<launcher_options::LauncherOptions>(
        launcher_options::LAUNCHER_OPTIONS_FILE, data.launcher_options
    );
}

void LauncherScene::on_awake() {
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoMove;
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
    colors[ImGuiCol_Text] = GRAYISH_TEXT;
    colors[ImGuiCol_WindowBg] = TRANSPARENT_BACKGROUND;
    colors[ImGuiCol_ChildBg] = BLACK_BACKGROUND;
    colors[ImGuiCol_PopupBg] = BLACK_BACKGROUND;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.FrameRounding = 4;
    style.WindowRounding = 6;
    style.ChildRounding = 6;
    style.PopupRounding = 6;
    style.GrabRounding = 6;
    style.GrabMinSize = 12;
    style.FramePadding = ImVec2(5.0f, 4.0f);

    ImGuiIO& io = ImGui::GetIO();

    // Setup Dear ImGui fonts
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"ă");
    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);

    using namespace sm::file_system;

    io.FontDefault = io.Fonts->AddFontFromFileTTF(path_for_assets(OPEN_SANS).c_str(), 21.0f, nullptr, ranges.Data);
    io.Fonts->Build();

    ctx->evt.connect<sm::WindowClosedEvent, &LauncherScene::on_window_closed>(this);
}

void LauncherScene::on_update() {
    float width, height, x_pos, y_pos;
    ctx->r2d->quad_center(width, height, x_pos, y_pos);

    auto background = objects.get<sm::gui::Image>("background"_H);
    background->set_position(glm::vec2(x_pos, y_pos));  // TODO it works, I have no idea why
    background->set_size(glm::vec2(width, height));
}

void LauncherScene::on_imgui_update() {
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Launcher", nullptr, window_flags);

    if (ImGui::BeginTabBar("tabs")) {
        page_display();
        page_graphics();

        ImGui::EndTabBar();
    }

    ImGui::Dummy(ImVec2(0.0f, 25.0f));
    ImGui::Dummy(ImVec2(90.0f, 0.0f));
    ImGui::SameLine();

    if (ImGui::Button("Play", ImVec2(120.0f, 32.0f))) {
        ctx->exit_code = 0;
        ctx->running = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Quit", ImVec2(120.0f, 32.0f))) {
        ctx->exit_code = 1;
        ctx->running = false;
    }

    ImGui::End();
}

void LauncherScene::on_window_closed(const sm::WindowClosedEvent&) {
    ctx->exit_code = 1;
}

void LauncherScene::page_display() {
    auto& data = ctx->data<Data>();

    if (ImGui::BeginTabItem("Display")) {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::PushItemWidth(170.0f);

        ImGui::Text("Fullscreen"); ImGui::SameLine();
        ImGui::Checkbox("##Fullscreen", &data.launcher_options.fullscreen);
        ImGui::SameLine(); help_marker("Window will be displayed on the primary monitor");

        ImGui::Text("Native Resolution"); ImGui::SameLine();
        ImGui::Checkbox("##Native Resolution", &data.launcher_options.native_resolution);
        ImGui::SameLine(); help_marker("Window will have the monitor's resolution instead of the resolution below");

        static size_t resolution_index = map_resolution_to_index(data.launcher_options.resolution);
        combo("Resolution", "##Resolution", resolution_index, get_available_resolutions(), [&data](size_t i) {
            data.launcher_options.resolution = map_index_to_resolution(i);
        });

        ImGui::Dummy(ImVec2(0.0f, 95.0f));

        ImGui::PopItemWidth();
        ImGui::EndTabItem();
    }
}

void LauncherScene::page_graphics() {
    auto& data = ctx->data<Data>();

    if (ImGui::BeginTabItem("Graphics")) {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::PushItemWidth(170.0f);

        static size_t samples_index = map_samples_to_index(data.launcher_options.samples);
        combo("Anti-Aliasing", "##Anti-Aliasing", samples_index, options_sample(), [&data](size_t i) {
            data.launcher_options.samples = map_index_to_samples(i);
        });

        static size_t anisotropic_fitering_index = map_anisotropic_filtering_to_index(data.launcher_options.anisotropic_filtering);
        combo("Anisotropic Filtering", "##Anisotropic Filtering", anisotropic_fitering_index, options_anisotropic_filtering(), [&data](size_t i) {
            data.launcher_options.anisotropic_filtering = map_index_to_anisotropic_filtering(i);
        });

        static size_t texture_quality_index = map_texture_quality_to_index(data.launcher_options.texture_quality);
        combo("Texture Quality", "##Texture Quality", texture_quality_index, { "Normal", "Low" }, [&data](size_t i) {
            data.launcher_options.texture_quality = map_index_to_texture_quality(i);
        });

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
}

void LauncherScene::initialize_resolutions() {
    const auto monitors = ctx->window->get_monitors();

    const auto resolution = monitors[0].get_resolution();  // Primary monitor's resolution
    const int width = resolution.first;

    if (width < 512) {
        LOG_DIST_CRITICAL("Monitor has unsupported resolution");
        sm::panic();
    } else if (width < 768) {
        resolutions_supported = 1;
    } else if (width < 1024) {
        resolutions_supported = 2;
    } else if (width < 1280) {
        resolutions_supported = 3;
    } else if (width < 1536) {
        resolutions_supported = 4;
    } else if (width < 1792) {
        resolutions_supported = 5;
    } else {
        resolutions_supported = 6;
    }
}

std::vector<const char*> LauncherScene::get_available_resolutions() {
    std::vector<const char*> resolutions;

    for (size_t i = 0; i < resolutions_supported; i++) {
        resolutions.push_back(RESOLUTIONS[i]);
    }

    return resolutions;
}
