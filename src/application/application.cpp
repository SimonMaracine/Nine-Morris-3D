#include <functional>
#include <memory>
#include <cassert>

#include "application/application.h"
#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/scene.h"
#include "application/input.h"
#include "graphics/renderer/renderer.h"
#include "graphics/debug_opengl.h"
#include "other/logging.h"

Application::Application(int width, int height, const std::string& title) {
    data.width = width;
    data.height = height;
    data.title = title;
    data.event_function = BIND(Application::on_event);

    Application::width = &data.width;
    Application::height = &data.height;

    logging::initialize();
    window = std::make_shared<Window>(&data);

#ifndef NDEBUG
    logging::log_opengl_and_dependencies_info(logging::LogTarget::Console);
#endif
    input::initialize(window->get_handle());
    debug_opengl::maybe_initialize_debugging();

    auto [version_major, version_minor] = debug_opengl::get_version_numbers();
    if (!(version_major == 4 && version_minor >= 3)) {
        REL_CRITICAL("Graphics card must support at minimum OpenGL 4.3 (it has {}.{})",
                version_major, version_minor);
        std::exit(1);
    }

    storage = renderer::initialize(this);
    assets_load = std::make_shared<AssetsLoad>();
}

Application::~Application() {
    for (unsigned int j = 0; j < scenes.size(); j++) {
        for (unsigned int i = 0; i < scenes[j]->layer_stack.size(); i++) {
            delete scenes[j]->layer_stack[i];
        }
        delete scenes[j];
    }

    renderer::terminate();
}

void Application::run() {
    assert(current_scene != nullptr);

    for (unsigned int j = 0; j < scenes.size(); j++) {
        for (unsigned int i = 0; i < scenes[j]->layer_stack.size(); i++) {
            scenes[j]->layer_stack[i]->on_bind_layers();
        }
    }

    current_scene->on_enter();
    for (unsigned int i = 0; i < current_scene->layer_stack.size(); i++) {
        current_scene->layer_stack[i]->on_attach();
    }

    SPDLOG_INFO("Initialized game");

    float dt = 0.0f;

    while (running) {
        dt = update_frame_counter();

        for (unsigned int i = 0; i < current_scene->layer_stack.size(); i++) {
            if (current_scene->layer_stack[i]->active) {
                current_scene->layer_stack[i]->on_update(dt);
                current_scene->layer_stack[i]->on_draw();
            }
        }

        if (changed_scene) {
            for (int i = current_scene->layer_stack.size() - 1; i >= 0; i--) {
                current_scene->layer_stack[i]->on_detach();
            }
            current_scene->on_exit();

            current_scene = to_scene;

            current_scene->on_enter();
            for (unsigned int i = 0; i < current_scene->layer_stack.size(); i++) {
                current_scene->layer_stack[i]->on_attach();
            }

            changed_scene = false;
        }

        window->update();
    }

    SPDLOG_INFO("Closing game");

    for (int i = current_scene->layer_stack.size() - 1; i >= 0; i--) {
        current_scene->layer_stack[i]->on_detach();
    }
    current_scene->on_exit();
}

void Application::add_scene(Scene* scene) {
    scenes.push_back(scene);
}

void Application::set_starting_scene(Scene* scene) {
    current_scene = scene;
}

void Application::change_scene(unsigned int id) {
    for (Scene* scene : scenes) {
        if (scene->id == id) {
            to_scene = scene;
            changed_scene = true;
            return;
        }
    }

    assert(false);
}

void Application::push_layer(Layer* layer, Scene* scene) {
    scene->layer_stack.push_back(layer);
}

void Application::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);
    dispatcher.dispatch<WindowClosedEvent>(WindowClosed, BIND(Application::on_window_closed));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(Application::on_window_resized));

    for (int i = current_scene->layer_stack.size() - 1; i >= 0; i--) {
        if (event.handled) {
            break;
        }

        if (current_scene->layer_stack[i]->active) {
            current_scene->layer_stack[i]->on_event(event);
        }
    }
}

int Application::get_width() {
    return *width;
}

int Application::get_height() {
    return *height;
}

float Application::update_frame_counter() {
    constexpr double MAX_DT = 1.0 / 20.0;

    static double previous_seconds = window->get_time();
    static int frame_count = 0;
    static double total_time = 0.0;

    const double current_seconds = window->get_time();
    const double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    total_time += elapsed_seconds;

    if (total_time > 0.25) {
        fps = (double) frame_count / total_time;
        frame_count = 0;
        total_time = 0.0;
    }
    frame_count++;

    const double delta_time = std::min(elapsed_seconds, MAX_DT);

    return (float) delta_time;
}

bool Application::on_window_closed(events::WindowClosedEvent& event) {
    running = false;

    return true;
}

bool Application::on_window_resized(events::WindowResizedEvent& event) {
    renderer::set_viewport(event.width, event.height);

    return false;
}

int* Application::width = nullptr;
int* Application::height = nullptr;
