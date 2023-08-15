#include <string_view>
#include <string>
#include <locale>
#include <memory>

#include <imgui.h>

#include "engine/application_base/application.hpp"
#include "engine/application_base/window.hpp"
#include "engine/application_base/events.hpp"
#include "engine/application_base/input.hpp"
#include "engine/application_base/application_builder.hpp"
#include "engine/application_base/panic.hpp"
#include "engine/application_base/platform.hpp"
#include "engine/audio/context.hpp"
#include "engine/audio/music.hpp"
#include "engine/graphics/renderer/renderer.hpp"
#include "engine/graphics/renderer/render_gl.hpp"
#include "engine/graphics/renderer/gui_renderer.hpp"
#include "engine/graphics/opengl/info_and_debug.hpp"
#include "engine/graphics/imgui_context.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"
#include "engine/other/encrypt.hpp"
#include "engine/other/file_system.hpp"
#include "engine/other/random_gen.hpp"
#include "engine/scene/scene.hpp"

namespace sm {
    void Application::initialize_applications(const ApplicationsData& data) {
        try {
            FileSystem::initialize_applications(data.app_name, data.res_directory);
        } catch (const FileSystem::UserNameError& e) {
            panic();  // Really bad that there is no feedback
        }

        // Set locale for the applications; used mostly by spdlog
        std::locale::global(std::locale("en_US.UTF-8"));

        Logging::initialize_applications(data.log_file, data.info_file);

#ifdef SM_BUILD_DISTRIBUTION
        FileSystem::check_and_fix_directories();
#endif
    }

    Application::Application(const ApplicationBuilder& builder, void* user_data) {
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
        properties.version_major = builder.major;
        properties.version_minor = builder.minor;
        properties.version_patch = builder.patch;
        properties.application = this;
        properties.ctx = &ctx;

        ctx.win = std::make_unique<Window>(this);
        ctx.inp.window_handle = ctx.win->get_handle();

        ImGuiContext::initialize(ctx.win->get_handle());

#ifndef SM_BUILD_DISTRIBUTION
        Logging::log_general_information(Logging::LogTarget::Console);
#endif

        GlInfoDebug::maybe_initialize_debugging();
        RenderGl::initialize_default();
        Encrypt::initialize(builder.encryption_key);

        const auto [version_major, version_minor] = GlInfoDebug::get_version_number();
        LOG_DIST_INFO("OpenGL version {}.{}", version_major, version_minor);

        ctx.r3d = std::make_unique<Renderer>(ctx.scr, properties.width, properties.height);

        // ctx.r2d = std::make_unique<Renderer2D>(properties.width, properties.height);

        if (builder.audio) {
            initialize_audio();
        }

        if (builder.random_generator) {
            initialize_random_generator();
        }

        ctx.evt.connect<WindowClosedEvent, &Application::on_window_closed>(this);
        ctx.evt.connect<WindowResizedEvent, &Application::on_window_resized>(this);

        frame_counter.previous_seconds = Window::get_time();
        fixed_update.previous_seconds = Window::get_time();
    }

    Application::~Application() {  // Destructor is called before all member variables
        ImGuiContext::uninitialize();
        MusicPlayer::uninitialize();
    }

    int Application::run(SceneId start_scene_id) {
        user_start_function();
        prepare_scenes(start_scene_id);
        on_start(current_scene);
        ctx.r3d->prerender_setup();

        ctx.win->show();
        LOG_INFO("Initialized application, entering main loop...");

        while (ctx.running) {
            ctx.delta = update_frame_counter();
            const unsigned int fixed_updates = calculate_fixed_update();

            for (unsigned int i = 0; i < fixed_updates; i++) {
                current_scene->on_fixed_update();
            }

            current_scene->on_update();
            ctx.tsk.update();

            // Clear the default framebuffer, as nobody does that for us
            RenderGl::clear(RenderGl::Buffers::C);

            ctx.r3d->render(properties.width, properties.height);
            // TODO r2d
            dear_imgui_render();

            ctx.win->update();
            ctx.evt.update();

            check_changed_scene();
        }

        LOG_INFO("Closing application...");

        current_scene->on_stop();
        ctx.r3d->postrender_setup();
        ctx.scr.clear_framebuffers();
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

        const double current_seconds = Window::get_time();
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

        const double current_seconds = Window::get_time();
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

            ctx.r3d->postrender_setup();

            // Initialize the new scene
            current_scene = to_scene;
            on_start(current_scene);

            // Clear all cached resources
            ctx.res = {};

            ctx.r3d->prerender_setup();

            changed_scene = false;
        }
    }

    void Application::dear_imgui_render() {
        ImGuiContext::begin_frame();

        current_scene->on_imgui_update();

        ImGuiContext::end_frame();
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
        ctx.scr.resize(event.width, event.height);
    }
}
