#include <functional>
#include <memory>
#include <cassert>

#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "application/layer.h"
#include "application/window.h"
#include "application/events.h"
#include "application/scene.h"
#include "application/input.h"
#include "graphics/renderer/renderer.h"
#include "graphics/debug_opengl.h"
#include "other/logging.h"

// Global reference to application
Application* app = nullptr;

Application::Application(int width, int height, const std::string& title) {
    data.width = width;
    data.height = height;
    data.title = title;
    data.event_function = BIND(Application::on_event);

    logging::initialize();
    window = std::make_shared<Window>(&data);

#ifdef NINE_MORRIS_3D_DEBUG
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
    assets_data = std::make_shared<AssetsData>();

    try {
        options::load_options_from_file(options);
    } catch (const options::OptionsFileNotOpenError& e) {
        REL_ERROR("{}", e.what());
        options::handle_options_file_not_open_error();
    } catch (const options::OptionsFileError& e) {
        REL_ERROR("{}", e.what());

        try {
            options::create_options_file();
        } catch (const options::OptionsFileNotOpenError& e) {
            REL_ERROR("{}", e.what());
        } catch (const options::OptionsFileError& e) {
            REL_ERROR("{}", e.what());
        }
    }
}

Application::~Application() {
    for (Scene* scene : scenes) {
        for (Layer* layer : scene->layer_stack) {
            delete layer;
        }
        delete scene;
    }

    renderer::terminate();
}

void Application::set_pointer(Application* instance) {
    assert(app == nullptr);
    app = instance;
}

void Application::run() {
    assert(current_scene != nullptr);
    assert(app != nullptr);

    for (Scene* scene : scenes) {
        for (Layer* layer : scene->layer_stack) {
            layer->on_bind_layers();
        }
    }

    current_scene->on_enter();
    for (Layer* layer : current_scene->layer_stack) {
        layer->on_attach();
    }

    DEB_INFO("Initialized game");

    while (running) {
        float dt = update_frame_counter();
        unsigned int fixed_updates = calculate_fixed_update();

        for (Layer* layer : current_scene->layer_stack) {
            if (layer->active) {
                for (unsigned int i = 0; i < fixed_updates; i++) {
                    layer->on_fixed_update();
                }
                layer->on_update(dt);
                layer->on_draw();
            }
        }

        if (changed_scene) {
            for (auto iter = current_scene->layer_stack.rbegin();
                    iter != current_scene->layer_stack.rend(); iter++) {
                (*iter)->on_detach();
            }
            current_scene->on_exit();

            current_scene = to_scene;

            current_scene->on_enter();
            for (Layer* layer : current_scene->layer_stack) {
                layer->on_attach();
            }

            changed_scene = false;
        }

        window->update();
    }

    DEB_INFO("Closing game");

    for (auto iter = current_scene->layer_stack.rbegin();
            iter != current_scene->layer_stack.rend(); iter++) {
        (*iter)->on_detach();
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

void Application::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);
    dispatcher.dispatch<WindowClosedEvent>(WindowClosed, BIND(Application::on_window_closed));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(Application::on_window_resized));

    for (auto iter = current_scene->layer_stack.rbegin();
            iter != current_scene->layer_stack.rend(); iter++) {
        if (event.handled) {
            break;
        }

        if ((*iter)->active) {
            (*iter)->on_event(event);
        }
    }
}

int Application::get_width() const {
    return data.width;
}

int Application::get_height() const {
    return data.height;
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

bool Application::on_window_closed(events::WindowClosedEvent& event) {
    running = false;

    return true;
}

bool Application::on_window_resized(events::WindowResizedEvent& event) {
    renderer::set_viewport(event.width, event.height);

    for (std::weak_ptr<Framebuffer> framebuffer : framebuffers) {
        std::shared_ptr<Framebuffer> fb = framebuffer.lock();
        if (fb != nullptr) {
            if (fb->get_specification().resizable) {
                fb->resize(event.width, event.height);
            }
        }
    }

    storage->orthographic_projection_matrix = glm::ortho(0.0f, static_cast<float>(event.width), 0.0f,
            static_cast<float>(event.height));

    storage->quad2d_shader->bind();
    storage->quad2d_shader->set_uniform_matrix("u_projection_matrix",
            storage->orthographic_projection_matrix);

    storage->text_shader->bind();
    storage->text_shader->set_uniform_matrix("u_projection_matrix",
            storage->orthographic_projection_matrix);

    return false;
}
