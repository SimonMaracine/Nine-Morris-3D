#include <functional>
#include <memory>

#include "application/application.h"
#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "opengl/renderer/renderer.h"

Application::Application(int width, int height, const std::string& title) {
    data.width = width;
    data.height = height;
    data.title = title;
    data.event_function = BIND(Application::on_event);

    window = std::make_shared<Window>(&data);
}

Application::~Application() {
    for (unsigned int i = 0; i < layer_stack.size(); i++) {
        layer_stack[i]->on_detach();
        delete layer_stack[i];
    }
}

void Application::run() {
    for (unsigned int i = 0; i < layer_stack.size(); i++) {
        layer_stack[i]->on_bind_layers();
    }

    float dt;

    while (running) {
        dt = update_frame_counter();

        for (unsigned int i = 0; i < layer_stack.size(); i++) {
            if (layer_stack[i]->active) {
                layer_stack[i]->on_update(dt);
                layer_stack[i]->on_draw();
            }
        }

        window->update();
    }
}

void Application::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);
    dispatcher.dispatch<WindowClosedEvent>(WindowClosed, BIND(Application::on_window_closed));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(Application::on_window_resized));

    for (int i = layer_stack.size() - 1; i >= 0; i--) {
        if (event.handled) {
            break;
        }

        if (layer_stack[i]->active) {
            layer_stack[i]->on_event(event);
        }
    }
}

float Application::update_frame_counter() {
    constexpr double MAX_DT = 1.0 / 20.0;

    static double previous_seconds = window->get_time();
    static int frame_count = 0;
    static double total_time = 0.0;

    double current_seconds = window->get_time();
    double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;

    total_time += elapsed_seconds;

    if (total_time > 0.25) {
        fps = (double) frame_count / total_time;
        frame_count = 0;
        total_time = 0.0;
    }
    frame_count++;

    double delta_time = std::min(elapsed_seconds, MAX_DT);

    return (float) delta_time;
}

void Application::push_layer(Layer* layer) {
    layer->on_attach();
    layer_stack.push_back(layer);
}

void Application::add_asset(unsigned int id, const std::string& file_path) {
    asset_manager.define_asset(id, file_path);
}

bool Application::on_window_closed(events::WindowClosedEvent& event) {
    running = false;

    return true;
}

bool Application::on_window_resized(events::WindowResizedEvent& event) {
    renderer::set_viewport(event.width, event.height);

    return false;
}
