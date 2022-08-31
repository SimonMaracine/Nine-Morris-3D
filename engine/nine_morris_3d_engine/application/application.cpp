#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "nine_morris_3d_engine/application/application.h"
// #include "nine_morris_3d_engine/application/layer.h"
#include "nine_morris_3d_engine/application/window.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/application/scene.h"
#include "nine_morris_3d_engine/application/input.h"
#include "nine_morris_3d_engine/ecs/internal_systems.h"
#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encryption.h"
#include "nine_morris_3d_engine/other/paths.h"

Application::Application(int width, int height, std::string_view title, std::string_view info_file,
        std::string_view log_file, std::string_view application_name) {
    app_data.width = width;
    app_data.height = height;
    app_data.title = title;
    // TODO use authors and version
    app_data.app = this;
    // app_data.event_function = BIND(Application::on_event);

    paths::initialize(application_name);
    logging::initialize(log_file);  // TODO move out in main
    window = std::make_unique<Window>(this);

#ifdef PLATFORM_GAME_DEBUG
    logging::log_opengl_and_dependencies_info(logging::LogTarget::Console, info_file);
#endif
    input::initialize(window->get_handle());
    debug_opengl::maybe_initialize_debugging();
    encryption::initialize();

    auto [version_major, version_minor] = debug_opengl::get_version_numbers();
    REL_INFO("Using OpenGL version {}.{}", version_major, version_minor);

    renderer = std::make_unique<Renderer>(this);
    gui_renderer = std::make_unique<GuiRenderer>(this);

    event_dispatcher.sink<WindowClosedEvent>().connect<&Application::on_window_closed>(*this);
    event_dispatcher.sink<WindowResizedEvent>().connect<&Application::on_window_resized>(*this);
    event_dispatcher.sink<MouseScrolledEvent>().connect<&Application::on_mouse_scrolled>(*this);
    event_dispatcher.sink<MouseMovedEvent>().connect<&Application::on_mouse_moved>(*this);

    // model_render_system(registry);  // TODO replace with the other API
    // quad_render_system(registry);
    // gui_image_system(registry);
    // gui_text_system(registry);
}

Application::~Application() {
    for (Scene* scene : scenes) {
        // for (Layer* layer : scene->overlays_in_order) {
        //     delete layer;
        // }
        // for (Layer* layer : scene->layers_in_order) {
        //     delete layer;
        // }
        delete scene;
    }
}

void Application::run() {
    ASSERT(current_scene != nullptr, "Starting scene not set");

    DEB_INFO("Starting game");

    // for (Layer* layer : current_scene->layers_in_order) {
    //     push_layer(layer);
    // }

    // for (Layer* layer : current_scene->overlays_in_order) {
    //     push_overlay(layer);
    // }

    on_start(current_scene);

    DEB_INFO("Initialized game, entering main loop");

    while (running) {
        delta = update_frame_counter();
        const unsigned int fixed_updates = calculate_fixed_update();

        // camera_system(registry, mouse_wheel, dx, dy, dt);  // TODO this should be user called

        // for (Layer* layer : active_layer_stack) {
        //     for (unsigned int i = 0; i < fixed_updates; i++) {
        //         layer->on_fixed_update();
        //     }
        //     layer->on_update(dt);
        // }
        for (unsigned int i = 0; i < fixed_updates; i++) {
            current_scene->on_fixed_update(this);
        }
        current_scene->on_update(this);

        renderer->render();
        gui_renderer->render();

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // for (Layer* layer : active_overlay_stack) {
        //     for (unsigned int i = 0; i < fixed_updates; i++) {
        //         layer->on_fixed_update();
        //     }
        //     layer->on_update(dt);
        // }
        current_scene->on_imgui_update(this);

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        event_dispatcher.update();
        window->update();

        check_changed_scene();
    }

    DEB_INFO("Closing game");

    current_scene->on_stop(this);

    // const size_t overlay_stack_size = _overlay_stack.size();
    // for (size_t i = 0; i < overlay_stack_size; i++) {
    //     pop_overlay();
    // }

    // const size_t layer_stack_size = _layer_stack.size();
    // for (size_t i = 0; i < layer_stack_size; i++) {
    //     pop_layer();
    // }
}

void Application::add_scene(Scene* scene, bool start) {
    scenes.push_back(scene);

    if (start) {
        ASSERT(current_scene == nullptr, "Cannot set two starting scenes");
        current_scene = scene;
    }
}

// void Application::set_starting_scene(Scene* scene) {
//     current_scene = scene;
// }

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

// void Application::update_active_layers() {
//     active_layer_stack.clear();
//     active_overlay_stack.clear();

//     for (Layer* layer : _layer_stack) {
//         if (layer->active) {
//             active_layer_stack.push_back(layer);
//         }
//     }

//     for (Layer* layer : _overlay_stack) {
//         if (layer->active) {
//             active_overlay_stack.push_back(layer);
//         }
//     }
// }

// void Application::on_event(events::Event& event) {
//     using namespace events;

//     Dispatcher dispatcher (event);

//     switch (event.get_type()) {
//         case WindowClosed:
//             dispatcher.dispatch<WindowClosedEvent>(BIND(Application::on_window_closed));
//             break;
//         case WindowResized:
//             dispatcher.dispatch<WindowResizedEvent>(BIND(Application::on_window_resized));
//             break;
//         case MouseScrolled:
//             dispatcher.dispatch<MouseScrolledEvent>(BIND(Application::on_mouse_scrolled));
//             break;
//         case MouseMoved:
//             dispatcher.dispatch<MouseMovedEvent>(BIND(Application::on_mouse_moved));
//             break;
//         default:
//             break;
//     }

    // for (auto iter = active_overlay_stack.rbegin(); iter != active_overlay_stack.rend(); iter++) {
    //     if (event.handled) {
    //         return;
    //     }

    //     (*iter)->on_event(event);
    // }

    // for (auto iter = active_layer_stack.rbegin(); iter != active_layer_stack.rend(); iter++) {
    //     if (event.handled) {
    //         return;
    //     }

    //     (*iter)->on_event(event);
    // }

//     current_scene->on_event(this, event);
// }

float Application::update_frame_counter() {
    constexpr double MAX_DT = 1.0 / 20.0;

    static double previous_seconds = window->get_time();
    static int frame_count = 0;
    static double total_time = 0.0;

    frames++;

    const double current_seconds = window->get_time();
    const double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    total_time += elapsed_seconds;

    if (total_time > 0.25) {
        fps = static_cast<double>(frame_count) / total_time;
        frame_count = 0;
        total_time = 0.0;
    }
    frame_count++;

    const double delta_time = std::min(elapsed_seconds, MAX_DT);

    return static_cast<float>(delta_time);
}

unsigned int Application::calculate_fixed_update() {
    constexpr double FIXED_DT = 1.0 / 50.0;

    static double previous_seconds = window->get_time();
    static double total_time = 0.0;

    const double current_seconds = window->get_time();
    const double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    total_time += elapsed_seconds;

    unsigned int updates = 0;

    while (true) {
        if (total_time > FIXED_DT) {
            total_time -= FIXED_DT;
            updates++;
        } else {
            break;
        }
    }

    return updates;
}

void Application::check_changed_scene() {
    if (changed_scene) {
        // active_layer_stack.clear();
        // active_overlay_stack.clear();

        // const size_t overlay_stack_size = _overlay_stack.size();
        // for (size_t i = 0; i < overlay_stack_size; i++) {
        //     pop_overlay();
        // }

        // const size_t layer_stack_size = _layer_stack.size();
        // for (size_t i = 0; i < layer_stack_size; i++) {
        //     pop_layer();
        // }

        current_scene->on_stop(this);

        current_scene = to_scene;

        on_start(current_scene);

        // for (Layer* layer : current_scene->layers_in_order) {
        //     push_layer(layer);
        // }

        // for (Layer* layer : current_scene->overlays_in_order) {
        //     push_overlay(layer);
        // }

        changed_scene = false;
    }
}

// void Application::push_layer(Layer* layer) {
//     _layer_stack.push_back(layer);
//     active_layer_stack.push_back(layer);

//     if (!layer->on_awake_called) {
//         layer->on_awake();
//         layer->on_awake_called = true;
//     }
//     layer->on_attach();
// }

// void Application::pop_layer() {
//     Layer* layer = _layer_stack.back();
//     layer->on_detach();
//     layer->active = true;

//     _layer_stack.pop_back();
// }

// void Application::push_overlay(Layer* layer) {
//     _overlay_stack.push_back(layer);
//     active_overlay_stack.push_back(layer);

//     if (!layer->on_awake_called) {
//         layer->on_awake();
//         layer->on_awake_called = true;
//     }
//     layer->on_attach();
// }

// void Application::pop_overlay() {
//     Layer* layer = _overlay_stack.back();
//     layer->on_detach();
//     layer->active = true;

//     _overlay_stack.pop_back();
// }

void Application::on_start(Scene* scene) {
    if (!scene->on_awake_called) {
        scene->on_awake(this);
        scene->on_awake_called = true;
    }
    scene->on_start(this);
}

void Application::on_window_closed(const WindowClosedEvent& event) {
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

    renderer->on_window_resized(event);
    gui_renderer->on_window_resized(event);
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
