#include <imgui.h>

#include "engine/application/application.h"
#include "engine/application/window.h"
#include "engine/application/events.h"
#include "engine/application/scene.h"
#include "engine/application/input.h"
#include "engine/application/application_builder.h"
#include "engine/audio/context.h"
#include "engine/audio/music.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/graphics/opengl/info_and_debug.h"
#include "engine/graphics/imgui_context.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/other/file_system.h"
#include "engine/other/exit.h"

std::any Application::dummy_user_data() {
    return std::make_any<DummyUserData>();
}

void Application::preinitialize(std::string_view app_name, std::string_view log_file, std::string_view info_file) {
    try {
        file_system::initialize_for_applications(app_name);
    } catch (const file_system::UserNameError& e) {
        REL_CRITICAL("Could not initialize file_system, exiting...");
        game_exit::exit_critical();
    }

    logging::initialize_for_applications(log_file, info_file);
}

Application::Application(const ApplicationBuilder& builder, std::any& user_data, const UserFunc& start, const UserFunc& stop)
    : builder(builder), _user_data(user_data), start(start), stop(stop) {
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
        DEB_INFO("With renderer ImGui");

        imgui_context::initialize(window);

        renderer_imgui_update = std::bind(&Application::renderer_imgui_func, this);

        evt.add_event<MouseScrolledEvent, &Application::on_imgui_mouse_scrolled>(this);
        evt.add_event<MouseMovedEvent, &Application::on_imgui_mouse_moved>(this);
        evt.add_event<MouseButtonPressedEvent, &Application::on_imgui_mouse_button_pressed>(this);
        evt.add_event<MouseButtonReleasedEvent, &Application::on_imgui_mouse_button_released>(this);
    }

#ifdef NM3D_PLATFORM_DEBUG
    logging::log_general_information(logging::LogTarget::Console);
#endif

    input::initialize(window->get_handle());
    gl::maybe_initialize_debugging();
    encrypt::initialize(builder.encryption_key);
    identifier::initialize();

    const auto [version_major, version_minor] = gl::get_version_number();
    REL_INFO("OpenGL version {}.{}", version_major, version_minor);

    if (builder.renderer_3d) {
        DEB_INFO("With renderer 3D");

        renderer = std::make_unique<Renderer>(this);
        renderer_3d_update = std::bind(&Application::renderer_3d_func, this);
    }

    if (builder.renderer_2d) {
        DEB_INFO("With renderer 2D");

        gui_renderer = std::make_unique<GuiRenderer>(this);
        renderer_2d_update = std::bind(&Application::renderer_2d_func, this);
    }

    if (builder.audio) {
        DEB_INFO("With audio");

        openal = std::make_unique<OpenAlContext>();
    }

    evt.add_event<WindowClosedEvent, &Application::on_window_closed>(this);
    evt.add_event<WindowResizedEvent, &Application::on_window_resized>(this);

    frame_counter.previous_seconds = window->get_time();
    fixed_update.previous_seconds = window->get_time();

    DEB_INFO("Calling user start routine...");
    start(this);
}

Application::~Application() {  // Destructor is called before all member variables
    DEB_INFO("Calling user stop routine...");
    stop(this);

    if (builder.renderer_imgui) {
        imgui_context::uninitialize();
    }

    music::stop_music_track();
}

int Application::run() {
    ASSERT(current_scene != nullptr, "Starting scene not set");

    prepare_scenes();
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

    return exit_code;
}

void Application::add_scene(std::unique_ptr<Scene>&& scene, bool start) {
    if (start) {
        ASSERT(current_scene == nullptr, "Cannot set two starting scenes");
        current_scene = scene.get();
    }

    scenes.push_back(std::move(scene));
}

void Application::change_scene(std::string_view name) {
    for (std::unique_ptr<Scene>& scene : scenes) {
        if (scene->name == name) {
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
    constexpr double MAX_DT = 1.0 / 20.0;

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
    constexpr double FIXED_DT = 1.0 / 50.0;

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
        current_scene = to_scene;
        on_start(current_scene);

        changed_scene = false;
    }
}

void Application::renderer_3d_func() {
    renderer->render();
}

void Application::renderer_2d_func() {
    gui_renderer->render();
}

void Application::renderer_imgui_func() {
    imgui_context::begin_frame();

    current_scene->on_imgui_update();

    imgui_context::end_frame();
}

void Application::prepare_scenes() {
    for (std::unique_ptr<Scene>& scene : scenes) {
        scene->app = this;
    }
}

void Application::on_start(Scene* scene) {
    if (!scene->on_awake_called) {
        scene->on_awake();
        scene->on_awake_called = true;
    }
    scene->on_start();
}

void Application::on_window_closed(const WindowClosedEvent&) {
    running = false;
}

void Application::on_window_resized(const WindowResizedEvent& event) {
    render_helpers::viewport(event.width, event.height);

    for (std::weak_ptr<gl::Framebuffer> framebuffer : framebuffers) {
        std::shared_ptr<gl::Framebuffer> fb = framebuffer.lock();
        if (fb != nullptr) {
            if (fb->get_specification().resizable) {
                fb->resize(event.width, event.height);
            }
        }
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
