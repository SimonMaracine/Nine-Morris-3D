#include <functional>
#include <memory>
#include <algorithm>
#include <string_view>
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "application/application.h"
#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/scene.h"
#include "application/input.h"
#include "application/extensions.h"
#include "graphics/renderer/main_renderer.h"
#include "graphics/renderer/gui_renderer.h"
#include "graphics/debug_opengl.h"
#include "other/logging.h"
#include "other/assert.h"

Application::Application(int width, int height, std::string_view title) {
    app_data.width = width;
    app_data.height = height;
    app_data.title = title;
    app_data.event_function = BIND(Application::on_event);

    logging::initialize();
    window = std::make_shared<Window>(&app_data);

#ifdef NINE_MORRIS_3D_DEBUG
    logging::log_opengl_and_dependencies_info(logging::LogTarget::Console);
#endif
    input::initialize(window->get_handle());
    debug_opengl::maybe_initialize_debugging();

    auto [version_major, version_minor] = debug_opengl::get_version_numbers();
    DEB_INFO("Using OpenGL version {}.{}", version_major, version_minor);

    if (extensions::extension_supported(extensions::AnisotropicFiltering)) {
        DEB_INFO("Anisotropic filtering is supported");
    } else {
        DEB_INFO("Anisotropic filtering is NOT supported");
    }

    renderer = std::make_unique<Renderer>(this);
    gui_renderer = std::make_unique<GuiRenderer>(this);
    assets_data = std::make_shared<AssetsData>();
}

Application::~Application() {
    for (Scene* scene : scenes) {
        for (Layer* layer : scene->overlays_in_order) {
            delete layer;
        }
        for (Layer* layer : scene->layers_in_order) {
            delete layer;
        }
        delete scene;
    }
}

void Application::run() {
    ASSERT(current_scene != nullptr, "Starting scene not set");

    for (Layer* layer : current_scene->layers_in_order) {
        push_layer(layer);
    }

    for (Layer* layer : current_scene->overlays_in_order) {
        push_overlay(layer);
    }

    DEB_INFO("Initialized game");

    while (running) {
        const float dt = update_frame_counter();
        const unsigned int fixed_updates = calculate_fixed_update();

        for (Layer* layer : active_layer_stack) {
            for (unsigned int i = 0; i < fixed_updates; i++) {
                layer->on_fixed_update();
            }
            layer->on_update(dt);
        }

        renderer->render();

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        for (Layer* layer : active_overlay_stack) {
            for (unsigned int i = 0; i < fixed_updates; i++) {
                layer->on_fixed_update();
            }
            layer->on_update(dt);
        }

        gui_renderer->render();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window->update();

        check_changed_scene();
    }

    DEB_INFO("Closing game");

    const size_t overlay_stack_size = _overlay_stack.size();
    for (size_t i = 0; i < overlay_stack_size; i++) {
        pop_overlay();
    }

    const size_t layer_stack_size = _layer_stack.size();
    for (size_t i = 0; i < layer_stack_size; i++) {
        pop_layer();
    }
}

void Application::add_scene(Scene* scene) {
    scenes.push_back(scene);
}

void Application::set_starting_scene(Scene* scene) {
    current_scene = scene;
}

void Application::change_scene(std::string_view id) {
    for (Scene* scene : scenes) {
        if (scene->id == id) {
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

    for (long i = framebuffers.size() - 1; i >= 0; i--) {
        for (size_t index : indices) {
            if (static_cast<long>(index) == i) {
                framebuffers.erase(std::next(framebuffers.begin(), index));
                break;
            }
        }
    }
}

void Application::update_active_layers() {
    active_layer_stack.clear();
    active_overlay_stack.clear();

    for (Layer* layer : _layer_stack) {
        if (layer->active) {
            active_layer_stack.push_back(layer);
        }
    }

    for (Layer* layer : _overlay_stack) {
        if (layer->active) {
            active_overlay_stack.push_back(layer);
        }
    }
}

void Application::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);
    dispatcher.dispatch<WindowClosedEvent>(WindowClosed, BIND(Application::on_window_closed));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(Application::on_window_resized));

    for (auto iter = active_overlay_stack.rbegin(); iter != active_overlay_stack.rend(); iter++) {
        if (event.handled) {
            return;
        }

        (*iter)->on_event(event);
    }

    for (auto iter = active_layer_stack.rbegin(); iter != active_layer_stack.rend(); iter++) {
        if (event.handled) {
            return;
        }

        (*iter)->on_event(event);
    }
}

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
        active_layer_stack.clear();
        active_overlay_stack.clear();

        const size_t overlay_stack_size = _overlay_stack.size();
        for (size_t i = 0; i < overlay_stack_size; i++) {
            pop_overlay();
        }

        const size_t layer_stack_size = _layer_stack.size();
        for (size_t i = 0; i < layer_stack_size; i++) {
            pop_layer();
        }

        current_scene = to_scene;

        for (Layer* layer : current_scene->layers_in_order) {
            push_layer(layer);
        }

        for (Layer* layer : current_scene->overlays_in_order) {
            push_overlay(layer);
        }

        changed_scene = false;
    }
}

void Application::push_layer(Layer* layer) {
    _layer_stack.push_back(layer);
    active_layer_stack.push_back(layer);

    if (!layer->on_awake_called) {
        layer->on_awake();
        layer->on_awake_called = true;
    }
    layer->on_attach();
}

void Application::pop_layer() {
    Layer* layer = _layer_stack.back();
    layer->on_detach();
    layer->active = true;

    _layer_stack.pop_back();
}

void Application::push_overlay(Layer* layer) {
    _overlay_stack.push_back(layer);
    active_overlay_stack.push_back(layer);

    if (!layer->on_awake_called) {
        layer->on_awake();
        layer->on_awake_called = true;
    }
    layer->on_attach();
}

void Application::pop_overlay() {
    Layer* layer = _overlay_stack.back();
    layer->on_detach();
    layer->active = true;

    _overlay_stack.pop_back();
}

bool Application::on_window_closed(events::WindowClosedEvent& event) {
    running = false;

    return true;
}

bool Application::on_window_resized(events::WindowResizedEvent& event) {
    renderer->set_viewport(event.width, event.height);

    for (std::weak_ptr<Framebuffer> framebuffer : framebuffers) {
        std::shared_ptr<Framebuffer> fb = framebuffer.lock();
        if (fb != nullptr) {
            if (fb->get_specification().resizable) {
                fb->resize(event.width, event.height);
            }
        }
    }

    camera.update_projection(static_cast<float>(event.width), static_cast<float>(event.height));

    gui_renderer->on_window_resized(event);

    return false;
}
