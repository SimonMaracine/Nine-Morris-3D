#include <imgui.h>

#include "engine/application/application.h"
#include "engine/application/window.h"
#include "engine/application/events.h"
#include "engine/application/input.h"
#include "engine/application/application_builder.h"
#include "engine/audio/context.h"
#include "engine/audio/music.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/render_helpers.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/graphics/opengl/info_and_debug.h"
#include "engine/graphics/imgui_context.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/other/file_system.h"
#include "engine/other/random_gen.h"
#include "engine/other/exit.h"
#include "engine/scene/scene.h"

void Application::preinitialize(std::string_view app_name, std::string_view log_file, std::string_view info_file) {
    try {
        file_system::initialize_for_applications(app_name);
    } catch (const file_system::UserNameError& e) {
        application_exit::panic();  // Really bad that there is no feedback
    }

    // Set locale for the applications; used mostly by spdlog
    std::locale::global(std::locale("en_US.UTF-8"));

    logging::initialize_for_applications(log_file, info_file);

#ifdef NM3D_PLATFORM_RELEASE
    file_system::check_and_fix_directories();
#endif
}

Application::Application(const ApplicationBuilder& builder, std::any& user_data, const UserFunc& start, const UserFunc& stop)
    : builder(builder), _user_data(&user_data), start(start), stop(stop) {
    DEB_INFO("Initializing application...");

    app_data.width = builder.width;
    app_data.height = builder.height;
    app_data.title = builder.title;
    app_data.fullscreen = builder.fullscreen;
    app_data.native_resolution = builder.native_resolution;
    app_data.resizable = builder.resizable;
    app_data.min_width = builder.min_width;
    app_data.min_height = builder.min_height;
    app_data.app_name = builder.app_name;
    app_data.authors = builder.author_list;  // TODO use this
    app_data.version_major = builder.major;
    app_data.version_minor = builder.minor;
    app_data.version_patch = builder.patch;
    app_data.app = this;

    window = std::make_unique<Window>(this);

    if (builder.renderer_imgui) {
        initialize_renderer_imgui();
    }

#ifdef NM3D_PLATFORM_DEBUG
    logging::log_general_information(logging::LogTarget::Console);
#endif

    input::initialize(window->get_handle());
    gl::maybe_initialize_debugging();
    render_helpers::initialize_default();
    encrypt::initialize(builder.encryption_key);
    identifier::initialize();
    random_gen::initialize();

    const auto [version_major, version_minor] = gl::get_version_number();
    REL_INFO("OpenGL version {}.{}", version_major, version_minor);

    if (builder.renderer_3d) {
        initialize_renderer_3d();
    }

    if (builder.renderer_2d) {
        initialize_renderer_2d();
    }

    if (builder.audio) {
        initialize_audio();
    }

    evt.add_event<WindowClosedEvent, &Application::on_window_closed>(this);
    evt.add_event<WindowResizedEvent, &Application::on_window_resized>(this);

    frame_counter.previous_seconds = window->get_time();
    fixed_update.previous_seconds = window->get_time();

    user_start();
}

Application::~Application() {  // Destructor is called before all member variables
    user_stop();

    if (builder.renderer_imgui) {
        imgui_context::uninitialize();
    }

    music::stop_music_track();
}

int Application::run(SceneId start_scene_id) {
    prepare_scenes(start_scene_id);

    on_start(current_scene);

    window->show();
    DEB_INFO("Initialized application, entering main loop...");

    while (running) {
        delta = update_frame_counter();
        const unsigned int fixed_updates = calculate_fixed_update();

        for (unsigned int i = 0; i < fixed_updates; i++) {
            current_scene->on_fixed_update();
        }

        current_scene->on_update();

        // Clear the default framebuffer, as nobody does that for us
        render_helpers::clear(render_helpers::Color);

        renderer_3d_update();
        renderer_2d_update();
        renderer_imgui_update();

        evt.update();
        window->update();

        check_changed_scene();
    }

    DEB_INFO("Closing application...");

    current_scene->on_stop();
    current_scene->_on_stop();

    return exit_code;
}

void Application::change_scene(SceneId id) {
    for (std::unique_ptr<Scene>& scene : scenes) {
        if (scene->id == id) {
            to_scene = scene.get();
            changed_scene = true;
            return;
        }
    }

    ASSERT(false, "Scene not found");
}

void Application::add_framebuffer(std::shared_ptr<gl::Framebuffer> framebuffer) {
    framebuffers.push_back(framebuffer);
}

void Application::purge_framebuffers() {
    std::vector<size_t> indices;

    for (size_t i = 0; i < framebuffers.size(); i++) {
        if (framebuffers[i].expired()) {
            indices.push_back(i);
        }
    }

    for (int64_t i = framebuffers.size() - 1; i >= 0; i--) {
        for (size_t index : indices) {
            if (static_cast<int64_t>(index) == i) {
                framebuffers.erase(std::next(framebuffers.begin(), index));
                break;
            }
        }
    }
}

float Application::update_frame_counter() {
    static constexpr double MAX_DT = 1.0 / 20.0;

    const double current_seconds = window->get_time();
    const double elapsed_seconds = current_seconds - frame_counter.previous_seconds;
    frame_counter.previous_seconds = current_seconds;

    frame_counter.total_time += elapsed_seconds;

    if (frame_counter.total_time > 0.25) {
        fps = static_cast<double>(frame_counter.frame_count) / frame_counter.total_time;
        frame_counter.frame_count = 0;
        frame_counter.total_time = 0.0;
    }
    frame_counter.frame_count++;

    const double delta_time = std::min(elapsed_seconds, MAX_DT);

    return static_cast<float>(delta_time);
}

unsigned int Application::calculate_fixed_update() {
    static constexpr double FIXED_DT = 1.0 / 50.0;

    const double current_seconds = window->get_time();
    const double elapsed_seconds = current_seconds - fixed_update.previous_seconds;
    fixed_update.previous_seconds = current_seconds;

    fixed_update.total_time += elapsed_seconds;

    unsigned int updates = 0;

    while (true) {
        if (fixed_update.total_time > FIXED_DT) {
            fixed_update.total_time -= FIXED_DT;
            updates++;
        } else {
            break;
        }
    }

    return updates;
}

void Application::check_changed_scene() {
    if (changed_scene) {
        current_scene->on_stop();
        current_scene->_on_stop();
        current_scene = to_scene;
        on_start(current_scene);

        changed_scene = false;
    }
}

void Application::renderer_3d_func() {
    renderer->render(current_scene->scene_list);
}

void Application::renderer_2d_func() {
    gui_renderer->render(current_scene->scene_list);
}

void Application::renderer_imgui_func() {
    imgui_context::begin_frame();

    current_scene->on_imgui_update();

    imgui_context::end_frame();
}

void Application::prepare_scenes(SceneId start_scene_id) {
    for (std::unique_ptr<Scene>& scene : scenes) {
        scene->app = this;

        if (scene->id == start_scene_id) {
            current_scene = scene.get();
        }
    }
}

void Application::on_start(Scene* scene) {
    if (!scene->on_awake_called) {
        scene->on_awake();
        scene->on_awake_called = true;
    }
    scene->on_start();
}

void Application::user_start() {
    DEB_INFO("Calling user start routine...");

    start(this);
}

void Application::user_stop() {
    DEB_INFO("Calling user stop routine...");

    stop(this);
}

void Application::initialize_renderer_3d() {
    DEB_INFO("With renderer 3D");

    renderer = std::make_unique<Renderer>(this);
    renderer_3d_update = std::bind(&Application::renderer_3d_func, this);
}

void Application::initialize_renderer_2d() {
    DEB_INFO("With renderer 2D");

    gui_renderer = std::make_unique<GuiRenderer>(this);
    renderer_2d_update = std::bind(&Application::renderer_2d_func, this);
}

void Application::initialize_renderer_imgui() {
    DEB_INFO("With renderer ImGui");

    imgui_context::initialize(window);

    renderer_imgui_update = std::bind(&Application::renderer_imgui_func, this);

    evt.add_event<MouseScrolledEvent, &Application::on_imgui_mouse_scrolled>(this);
    evt.add_event<MouseMovedEvent, &Application::on_imgui_mouse_moved>(this);
    evt.add_event<MouseButtonPressedEvent, &Application::on_imgui_mouse_button_pressed>(this);
    evt.add_event<MouseButtonReleasedEvent, &Application::on_imgui_mouse_button_released>(this);
}

void Application::initialize_audio() {
    DEB_INFO("With audio");

    openal = std::make_unique<OpenAlContext>();
}

void Application::on_window_closed(const WindowClosedEvent&) {
    running = false;
}

void Application::on_window_resized(const WindowResizedEvent& event) {
    if (event.width == 0 || event.height == 0) {  // TODO maybe rework event system
        return;
    }

    render_helpers::viewport(event.width, event.height);

    for (std::weak_ptr<gl::Framebuffer> framebuffer : framebuffers) {
        std::shared_ptr<gl::Framebuffer> fb = framebuffer.lock();

        if (fb == nullptr) {
            return;
        }

        if (!fb->get_specification().resizable) {
            return;
        }

        fb->resize(event.width, event.height);
    }
}

void Application::on_imgui_mouse_scrolled(const MouseScrolledEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = event.scroll;
}

void Application::on_imgui_mouse_moved(const MouseMovedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event.mouse_x, event.mouse_y);
}

void Application::on_imgui_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[static_cast<int>(event.button)] = true;
}

void Application::on_imgui_mouse_button_released(const MouseButtonReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[static_cast<int>(event.button)] = false;
}
