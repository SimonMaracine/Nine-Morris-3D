#include "engine/application_base/application.hpp"

#include <locale>
#include <memory>
#include <algorithm>
#include <cassert>

#include "engine/application_base/input.hpp"
#include "engine/application_base/platform.hpp"
#include "engine/audio/music.hpp"
#include "engine/graphics/opengl/opengl.hpp"
#include "engine/graphics/opengl/info_and_debug.hpp"
#include "engine/graphics/dear_imgui_context.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/file_system.hpp"
#include "engine/other/random_gen.hpp"

namespace sm {
    bool Application::initialize_applications(const ApplicationsData& data) {
        if (!FileSystem::initialize_applications(data.app_name, data.res_directory)) {
            return false;
        }

        // Set locale for the applications; used mostly by spdlog
        std::locale::global(std::locale("en_US.UTF-8"));

        Logging::initialize_applications(data.log_file, data.info_file);

#ifdef SM_BUILD_DISTRIBUTION
        FileSystem::check_and_fix_directories();
#endif

        return true;
    }

    Application::Application(const ApplicationProperties& properties) {
        LOG_INFO("Initializing application...");

        ctx.application = this;
        ctx.user_data = properties.user_data;
        ctx.win = std::make_unique<Window>(properties, &ctx);

        Input::initialize(ctx.win->get_handle());
        DearImGuiContext::initialize(ctx.win->get_handle());

#ifndef SM_BUILD_DISTRIBUTION
        Logging::log_general_information(Logging::LogTarget::Console);
#endif

        GlInfoDebug::maybe_initialize_debugging();
        OpenGl::initialize_default();

        const auto [version_major, version_minor] {GlInfoDebug::get_version_number()};
        LOG_DIST_INFO("OpenGL version {}.{}", version_major, version_minor);

        ctx.rnd = std::make_unique<Renderer>(properties.width, properties.height);

        if (properties.audio) {
            initialize_audio();
        }

        ctx.evt.connect<WindowClosedEvent, &Application::on_window_closed>(this);
        ctx.evt.connect<WindowResizedEvent, &Application::on_window_resized>(this);

        frame_counter.previous_seconds = Window::get_time();
        fixed_update.previous_seconds = Window::get_time();
    }

    Application::~Application() {  // Destructor is called before all member variables
        MusicPlayer::uninitialize();
        DearImGuiContext::uninitialize();
        Input::uninitialize();
    }

    int Application::run(SceneId start_scene_id) {
        prepare_scenes(start_scene_id);

        user_start_function();
        current_scene->on_start();
        ctx.rnd->prerender_setup();

        ctx.win->show();
        LOG_INFO("Initialized application, entering main loop...");

        while (ctx.running) {
            ctx.delta = update_frame_counter();
            const unsigned int fixed_updates {calculate_fixed_update()};

            for (unsigned int i {0}; i < fixed_updates; i++) {
                current_scene->on_fixed_update();
            }

            current_scene->on_update();
            ctx.tsk.update();

            ctx.rnd->render(ctx.win->get_width(), ctx.win->get_height());
            dear_imgui_render();

            ctx.win->update();
            ctx.evt.update();

            check_changed_scene();
        }

        LOG_INFO("Closing application...");

        ctx.rnd->postrender_setup();
        current_scene->on_stop();
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
        static constexpr double MAX_DT {1.0 / 20.0};

        const double current_seconds {Window::get_time()};
        const double elapsed_seconds {current_seconds - frame_counter.previous_seconds};
        frame_counter.previous_seconds = current_seconds;

        frame_counter.total_time += elapsed_seconds;

        if (frame_counter.total_time > 0.25) {
            ctx.fps = static_cast<double>(frame_counter.frame_count) / frame_counter.total_time;
            frame_counter.frame_count = 0;
            frame_counter.total_time = 0.0;
        }
        frame_counter.frame_count++;

        const double delta_time {std::min(elapsed_seconds, MAX_DT)};

        return static_cast<float>(delta_time);
    }

    unsigned int Application::calculate_fixed_update() {
        static constexpr double FIXED_DT {1.0 / 50.0};

        const double current_seconds {Window::get_time()};
        const double elapsed_seconds {current_seconds - fixed_update.previous_seconds};
        fixed_update.previous_seconds = current_seconds;

        fixed_update.total_time += elapsed_seconds;

        unsigned int updates {0};

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
        if (next_scene != nullptr) {
            ctx.rnd->postrender_setup();
            current_scene->on_stop();

            // Clear all cached resources
            ctx.res.clear();

            // Disconnect all scene callbacks to events
            ctx.evt.disconnect(current_scene);

            // Set and initialize the new scene
            current_scene = next_scene;
            next_scene = nullptr;

            current_scene->on_start();
            ctx.rnd->prerender_setup();
        }

        assert(next_scene == nullptr);
    }

    void Application::dear_imgui_render() {
        DearImGuiContext::begin_frame();
        current_scene->on_imgui_update();
        DearImGuiContext::end_frame();
    }

    void Application::prepare_scenes(SceneId start_scene_id) {
        for (std::unique_ptr<Scene>& scene : scenes) {
            scene->ctx = &ctx;

            if (scene->id == start_scene_id) {
                current_scene = scene.get();
            }
        }

        assert(current_scene != nullptr);
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

    void Application::on_window_closed(const WindowClosedEvent&) {
        ctx.running = false;
    }

    void Application::on_window_resized(const WindowResizedEvent& event) {
        ctx.rnd->resize_framebuffers(event.width, event.height);
    }
}
