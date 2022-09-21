#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "nine_morris_3d_engine/application/application.h"
#include "nine_morris_3d_engine/application/window.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/application/scene.h"
#include "nine_morris_3d_engine/application/input.h"
#include "nine_morris_3d_engine/application/application_builder.h"
#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encrypt.h"
#include "nine_morris_3d_engine/other/paths.h"

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
    app_data.application_name = builder.application_name;
    app_data.info_file_name = builder.info_file_name;
    app_data.authors = builder.authors;
    app_data.version_major = builder.major;
    app_data.version_minor = builder.minor;
    app_data.version_patch = builder.patch;
    app_data.app = this;

    paths::initialize(builder.application_name);
    window = std::make_unique<Window>(this);

    if (builder.renderer_imgui) {
        DEB_INFO("With renderer ImGui");

        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init("#version 430 core");
        ImGui_ImplGlfw_InitForOpenGL(window->get_handle(), false);

        renderer_imgui = std::bind(&Application::renderer_imgui_functionality, this);        

        evt.sink<MouseScrolledEvent>().connect<&Application::on_imgui_mouse_scrolled>(*this);
        evt.sink<MouseMovedEvent>().connect<&Application::on_imgui_mouse_moved>(*this);
        evt.sink<MouseButtonPressedEvent>().connect<&Application::on_imgui_mouse_button_pressed>(*this);
        evt.sink<MouseButtonReleasedEvent>().connect<&Application::on_imgui_mouse_button_released>(*this);
    }

#ifdef PLATFORM_GAME_DEBUG
    logging::log_opengl_and_dependencies_info(logging::LogTarget::Console, builder.info_file_name);
#endif
    input::initialize(window->get_handle());
    debug_opengl::maybe_initialize_debugging();
    encrypt::initialize(builder.encrypt_key);

    auto [version_major, version_minor] = debug_opengl::get_version_numbers();
    REL_INFO("Using OpenGL version {}.{}", version_major, version_minor);

    if (builder.renderer_3d) {
        DEB_INFO("With renderer 3D");

        renderer = std::make_unique<Renderer>(this);
        renderer_3d = std::bind(&Application::renderer_3d_functionality, this);        
    }
    
    if (builder.renderer_2d) {
        DEB_INFO("With renderer 2D");

        gui_renderer = std::make_unique<GuiRenderer>(this);
        renderer_2d = std::bind(&Application::renderer_2d_functionality, this);
    }

    evt.sink<WindowClosedEvent>().connect<&Application::on_window_closed>(*this);
    evt.sink<WindowResizedEvent>().connect<&Application::on_window_resized>(*this);
    evt.sink<MouseScrolledEvent>().connect<&Application::on_mouse_scrolled>(*this);
    evt.sink<MouseMovedEvent>().connect<&Application::on_mouse_moved>(*this);

    DEB_INFO("Calling user start routine...");
    start(this);

    frame_counter.previous_seconds = window->get_time();
    fixed_update.previous_seconds = window->get_time();

    // model_render_system(registry);  // TODO replace with the other API
    // quad_render_system(registry);
    // gui_image_system(registry);
    // gui_text_system(registry);
}

Application::~Application() {
    DEB_INFO("Calling user stop routine...");
    stop(this);

    if (builder.renderer_imgui) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    for (Scene* scene : scenes) {  // TODO maybe use unique_ptr instead
        delete scene;
    }
}

int Application::run() {
    ASSERT(current_scene != nullptr, "Starting scene not set");

    prepare_scenes();
    on_start(current_scene);

    DEB_INFO("Initialized application, entering main loop...");

    while (running) {
        delta = update_frame_counter();
        const unsigned int fixed_updates = calculate_fixed_update();

        // camera_system(registry, mouse_wheel, dx, dy, dt);  // TODO this should be user called

        for (unsigned int i = 0; i < fixed_updates; i++) {
            current_scene->on_fixed_update();
        }
        current_scene->on_update();

        render_helpers::clear(render_helpers::Color);

        renderer_3d();
        renderer_2d();
        renderer_imgui();

        evt.update();
        window->update();

        check_changed_scene();
    }

    DEB_INFO("Closing application...");

    current_scene->on_stop();

    return exit_code;
}

void Application::add_scene(Scene* scene, bool start) {
    scenes.push_back(scene);

    if (start) {
        ASSERT(current_scene == nullptr, "Cannot set two starting scenes");
        current_scene = scene;
    }
}

void Application::change_scene(std::string_view name) {
    for (Scene* scene : scenes) {
        if (scene->name == name) {
            to_scene = scene;
            changed_scene = true;
            return;
        }
    }

    ASSERT(false, "Scene not found");
}

void Application::add_framebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    framebuffers.push_back(static_cast<std::shared_ptr<Framebuffer>>(framebuffer));
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

    frames++;

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

void Application::renderer_3d_functionality() {
    renderer->render();
}

void Application::renderer_2d_functionality() {
    gui_renderer->render();
}

void Application::renderer_imgui_functionality() {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    current_scene->on_imgui_update();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::prepare_scenes() {
    for (Scene* scene : scenes) {
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

    for (std::weak_ptr<Framebuffer> framebuffer : framebuffers) {
        std::shared_ptr<Framebuffer> fb = framebuffer.lock();
        if (fb != nullptr) {
            if (fb->get_specification().resizable) {
                fb->resize(event.width, event.height);
            }
        }
    }

    // camera.update_projection(static_cast<float>(event.width), static_cast<float>(event.height));
    // camera_projection_system(registry, event.width, event.height);  // TODO this should be user called

    if (builder.renderer_3d) {
        renderer->on_window_resized(event);
    }

    if (builder.renderer_2d) {
        gui_renderer->on_window_resized(event);
    }
}

void Application::on_mouse_scrolled(const MouseScrolledEvent& event) {
    mouse_wheel = event.scroll;
}

void Application::on_mouse_moved(const MouseMovedEvent& event) {
    dx = last_mouse_x - event.mouse_x;
    dy = last_mouse_y - event.mouse_y;
    last_mouse_x = event.mouse_x;
    last_mouse_y = event.mouse_y;
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

std::any dummy_user_data() {
    return std::make_any<_DummyUserData>();
}
