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
#include "engine/dear_imgui/imgui_context.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/other/file_system.h"
#include "engine/other/random_gen.h"
#include "engine/application/panic.h"
#include "engine/scene/scene.h"

void Application::preinitialize(std::string_view app_name, std::string_view log_file, std::string_view info_file) {
    try {
        file_system::initialize_for_applications(app_name);
    } catch (const file_system::UserNameError& e) {
        panic::panic();  // Really bad that there is no feedback
    }

    // Set locale for the applications; used mostly by spdlog
    std::locale::global(std::locale("en_US.UTF-8"));

    logging::initialize_for_applications(log_file, info_file);

#ifdef NM3D_PLATFORM_DISTRIBUTION
    file_system::check_and_fix_directories();
#endif
}

Application::Application(const ApplicationBuilder& builder, void* user_data)
    : builder(builder) {
    LOG_INFO("Initializing application...");

    ctx.application = this;
    ctx.user_data = user_data;
    ctx.properties = &properties;

    properties.width = builder.width;
    properties.height = builder.height;
    properties.title = builder.title;
    properties.fullscreen = builder.fullscreen;
    properties.native_resolution = builder.native_resolution;
    properties.resizable = builder.resizable;
    properties.min_width = builder.min_width;
    properties.min_height = builder.min_height;
    properties.app_name = builder.app_name;
    properties.authors = builder.author_list;  // TODO use this
    properties.version_major = builder.major;
    properties.version_minor = builder.minor;
    properties.version_patch = builder.patch;
    properties.application = this;
    properties.ctx = &ctx;

    ctx.window = std::make_unique<Window>(this);
    ctx.input.window_handle = ctx.window->get_handle();

    if (builder.dear_imgui) {
        initialize_dear_imgui();
    }

#ifndef NM3D_PLATFORM_DISTRIBUTION
    logging::log_general_information(logging::LogTarget::Console);
#endif

    gl::maybe_initialize_debugging();
    render_helpers::initialize_default();
    Encrypt::initialize(builder.encryption_key);

    const auto [version_major, version_minor] = gl::get_version_number();
    LOG_DIST_INFO("OpenGL version {}.{}", version_major, version_minor);

    if (builder.renderer_3d) {
        initialize_r3d();
    }

    if (builder.renderer_2d) {
        initialize_r2d();
    }

    if (builder.audio) {
        initialize_audio();
    }

    if (builder.random_generator) {
        initialize_random_generator();
    }

    ctx.evt.connect<WindowClosedEvent, &Application::on_window_closed>(this);
    ctx.evt.connect<WindowResizedEvent, &Application::on_window_resized>(this);

    frame_counter.previous_seconds = ctx.window->get_time();
    fixed_update.previous_seconds = ctx.window->get_time();
}

Application::~Application() {  // Destructor is called before all member variables
    if (builder.dear_imgui) {
        imgui_context::uninitialize();
    }

    music::stop_music_track();
}

int Application::run(SceneId start_scene_id) {
    user_start_function();

    prepare_scenes(start_scene_id);

    on_start(current_scene);

    ctx.window->show();
    LOG_INFO("Initialized application, entering main loop...");

    while (ctx.running) {
        ctx.delta = update_frame_counter();
        const unsigned int fixed_updates = calculate_fixed_update();

        for (unsigned int i = 0; i < fixed_updates; i++) {
            current_scene->on_fixed_update();
        }

        current_scene->on_update();

        // Clear the default framebuffer, as nobody does that for us
        render_helpers::clear(render_helpers::Color);

        r3d_update();
        r2d_update();
        dear_imgui_update();

        ctx.window->update();
        ctx.evt.update();

        check_changed_scene();
    }

    LOG_INFO("Closing application...");

    current_scene->on_stop();
    current_scene->_on_stop();

    user_stop_function();

    return ctx.exit_code;
}

void Application::set_start_function(const UserFunc& start) {
    this->start = start;
}

void Application::set_stop_function(const UserFunc& stop) {
    this->stop = stop;
}

float Application::update_frame_counter() {
    static constexpr double MAX_DT = 1.0 / 20.0;

    const double current_seconds = ctx.window->get_time();
    const double elapsed_seconds = current_seconds - frame_counter.previous_seconds;
    frame_counter.previous_seconds = current_seconds;

    frame_counter.total_time += elapsed_seconds;

    if (frame_counter.total_time > 0.25) {
        ctx.fps = static_cast<double>(frame_counter.frame_count) / frame_counter.total_time;
        frame_counter.frame_count = 0;
        frame_counter.total_time = 0.0;
    }
    frame_counter.frame_count++;

    const double delta_time = std::min(elapsed_seconds, MAX_DT);

    return static_cast<float>(delta_time);
}

unsigned int Application::calculate_fixed_update() {
    static constexpr double FIXED_DT = 1.0 / 50.0;

    const double current_seconds = ctx.window->get_time();
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

void Application::r3d_function() {
    ctx.r3d->render(current_scene->scene_list);
}

void Application::r2d_function() {
    ctx.r2d->render(current_scene->scene_list);
}

void Application::dear_imgui_function() {
    imgui_context::begin_frame();

    current_scene->on_imgui_update();

    imgui_context::end_frame();
}

void Application::prepare_scenes(SceneId start_scene_id) {
    for (std::unique_ptr<Scene>& scene : scenes) {
        scene->ctx = &ctx;

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

void Application::user_start_function() {
    LOG_INFO("Calling user start routine...");

    start(&ctx);
}

void Application::user_stop_function() {
    LOG_INFO("Calling user stop routine...");

    stop(&ctx);
}

void Application::initialize_r3d() {
    LOG_INFO("With renderer 3D");

    ctx.r3d = std::make_unique<Renderer>(&ctx);
    r3d_update = std::bind(&Application::r3d_function, this);
}

void Application::initialize_r2d() {
    LOG_INFO("With renderer 2D");

    ctx.r2d = std::make_unique<GuiRenderer>(&ctx);
    r2d_update = std::bind(&Application::r2d_function, this);
}

void Application::initialize_dear_imgui() {
    LOG_INFO("With renderer Dear ImGui");

    imgui_context::initialize(ctx.window->get_handle());

    dear_imgui_update = std::bind(&Application::dear_imgui_function, this);
}

void Application::initialize_audio() {
    LOG_INFO("With audio");

    ctx.snd = std::make_unique<OpenAlContext>();
}

void Application::initialize_random_generator() {
    LOG_INFO("With random number generator");

    ctx.rng = std::make_unique<RandomGenerator>();
}

void Application::on_window_closed(const WindowClosedEvent&) {
    ctx.running = false;
}

void Application::on_window_resized(const WindowResizedEvent& event) {
    if (event.width == 0 || event.height == 0) {
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
