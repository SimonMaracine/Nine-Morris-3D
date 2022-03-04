#include <functional>
#include <memory>
#include <cassert>
#include <algorithm>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/scene.h"
#include "application/input.h"
#include "graphics/renderer/new_renderer.h"
#include "graphics/debug_opengl.h"
#include "other/logging.h"

Application::Application(int width, int height, const std::string& title) {
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
    if (!(version_major == 4 && version_minor >= 3)) {
        REL_CRITICAL("Graphics card must support at minimum OpenGL 4.3 (it has {}.{})",
                version_major, version_minor);
        exit(1);
    }

    // storage = renderer::initialize(this);
    renderer = std::make_unique<Renderer>(this);
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

    // renderer::terminate();
}

void Application::run() {
    assert(current_scene != nullptr);

    for (Scene* scene : scenes) {
        for (Layer* layer : scene->layers_in_order) {
            layer->on_bind_layers();
        }
    }

    for (Scene* scene : scenes) {
        for (Layer* layer : scene->overlays_in_order) {
            layer->on_bind_layers();
        }
    }

    for (Layer* layer : current_scene->layers_in_order) {
        push_layer(layer);
    }

    for (Layer* layer : current_scene->overlays_in_order) {
        push_overlay(layer);
    }

    DEB_INFO("Initialized game");

    while (running) {
        float dt = update_frame_counter();
        unsigned int fixed_updates = calculate_fixed_update();

        for (Layer* layer : active_layer_stack) {
            for (unsigned int i = 0; i < fixed_updates; i++) {
                layer->on_fixed_update();
            }
            layer->on_update(dt);
            layer->on_draw();  // TODO maybe get rid of on_draw and keep only on_update
        }

        renderer->render();

        for (Layer* layer : active_overlay_stack) {
            for (unsigned int i = 0; i < fixed_updates; i++) {
                layer->on_fixed_update();
            }
            layer->on_update(dt);
            layer->on_draw();  // TODO maybe get rid of on_draw and keep only on_update
        }

        window->update();

        if (changed_scene) {
            active_layer_stack.clear();

            for (auto iter = overlay_stack.rbegin(); iter != overlay_stack.rend(); iter++) {
                pop_overlay(*iter);
            }

            for (auto iter = layer_stack.rbegin(); iter != layer_stack.rend(); iter++) {
                pop_layer(*iter);
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

    DEB_INFO("Closing game");

    for (auto iter = overlay_stack.rbegin(); iter != overlay_stack.rend(); iter++) {
        pop_overlay(*iter);
    }

    for (auto iter = layer_stack.rbegin(); iter != layer_stack.rend(); iter++) {
        pop_layer(*iter);
    }
}

void Application::add_scene(Scene* scene) {
    scenes.push_back(scene);
}

void Application::set_starting_scene(Scene* scene) {
    current_scene = scene;
}

void Application::change_scene(const std::string& id) {
    for (Scene* scene : scenes) {
        if (scene->id == id) {
            to_scene = scene;
            changed_scene = true;
            return;
        }
    }

    assert(false);
}

void Application::add_framebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    framebuffers.push_back(framebuffer);
}

void Application::purge_framebuffers() {
    for (auto iter = framebuffers.rbegin(); iter != framebuffers.rend(); iter++) {
        if (iter->expired()) {
            iter = decltype(iter)(framebuffers.erase(std::next(iter).base()));
        }
    }
}

void Application::update_active_layers() {
    active_layer_stack.clear();
    active_overlay_stack.clear();

    for (Layer* layer : layer_stack) {
        if (layer->active) {
            active_layer_stack.push_back(layer);
        }
    }

    for (Layer* layer : overlay_stack) {
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

void Application::push_layer(Layer* layer) {
    layer_stack.push_back(layer);
    active_layer_stack.push_back(layer);
    layer->on_attach();
}

void Application::pop_layer(Layer* layer) {
    layer->on_detach();
    auto iter = std::find(layer_stack.begin(), layer_stack.end(), layer);
    layer_stack.erase(iter);
    layer->active = true;
}

void Application::push_overlay(Layer* layer) {
    overlay_stack.push_back(layer);
    active_overlay_stack.push_back(layer);
    layer->on_attach();
}

void Application::pop_overlay(Layer* layer) {
    layer->on_detach();
    auto iter = std::find(overlay_stack.begin(), overlay_stack.end(), layer);
    overlay_stack.erase(iter);
    layer->active = true;
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

    renderer->storage.orthographic_projection_matrix = glm::ortho(0.0f, static_cast<float>(event.width), 0.0f,
            static_cast<float>(event.height));

    renderer->storage.quad2d_shader->bind();  // TODO optimize
    renderer->storage.quad2d_shader->set_uniform_mat4("u_projection_matrix",
            renderer->storage.orthographic_projection_matrix);

    renderer->storage.text_shader->bind();
    renderer->storage.text_shader->set_uniform_mat4("u_projection_matrix",
            renderer->storage.orthographic_projection_matrix);

    return false;
}
