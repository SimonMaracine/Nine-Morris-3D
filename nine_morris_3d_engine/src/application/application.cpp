#include "nine_morris_3d_engine/application/application.hpp"

#include <memory>
#include <algorithm>
#include <utility>
#include <cassert>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/graphics/internal/imgui_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/opengl.hpp"
#include "nine_morris_3d_engine/graphics/opengl/debug.hpp"
#include "nine_morris_3d_engine/graphics/opengl/capabilities.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    Application::Application(const ApplicationProperties& properties)
        : ctx(properties) {
        ctx.application = this;
        ctx.user_data = properties.user_data;

        internal::imgui_context::initialize(ctx.win.get_handle());

        LOG_DIST_INFO("Working directory: {}", internal::FileSystem::current_working_directory());

        const auto [version_major, version_minor] {opengl_debug::get_version_number()};
        LOG_DIST_INFO("GL version {}.{}", version_major, version_minor);

        if (!capabilities::is_srgb_capable()) {
            LOG_DIST_WARNING("Default GL framebuffer is not sRGB capable");
        }

#ifndef SM_BUILD_DISTRIBUTION
        LOG_INFO("{}", ctx.get_information());
#endif

        ctx.evt.connect<WindowClosedEvent, &Application::on_window_closed>(this);
        ctx.evt.connect<WindowResizedEvent, &Application::on_window_resized>(this);
        ctx.evt.connect<WindowIconifiedEvent, &Application::on_window_iconified>(this);

        frame_counter.previous_seconds = internal::Window::get_time();
        fixed_update.previous_seconds = internal::Window::get_time();
    }

    Application::~Application() {  // Destructor is called before all member variables
        internal::imgui_context::uninitialize();
    }

    int Application::run(Id start_scene_id, const UserFunctions& user_functions) {
        setup_start_scene(start_scene_id);

        LOG_INFO("Calling user start function...");

        user_functions.start(ctx);

        LOG_INFO("Calling scene on_start...");

        current_scene->on_start();
        ctx.rnd.pre_setup();

        ctx.win.show();

        LOG_INFO("Entering application main loop...");

        while (ctx.running) {
            ctx.delta = update_frame_counter();
            const unsigned int fixed_updates {calculate_fixed_update()};

            for (unsigned int i {0}; i < fixed_updates; i++) {
                current_scene->on_fixed_update();
            }

            current_scene->on_update();
            ctx.tsk.update();

            if (!minimized) {
                ctx.dbg.add_lines(ctx.scn);
                ctx.rnd.render(ctx.scn, ctx.win.get_width(), ctx.win.get_height());
                dear_imgui_render();
            }

            ctx.scn.clear();

            ctx.win.update();
            ctx.evt.update();

            check_changed_scene();
        }

        LOG_INFO("Closing application...");

        ctx.rnd.post_setup();

        LOG_INFO("Calling scene on_stop...");

        current_scene->on_stop();

        LOG_INFO("Calling user stop function...");

        user_functions.stop(ctx);

        return ctx.exit_code;
    }

    float Application::update_frame_counter() {
        static constexpr double MAX_DT {1.0 / 20.0};

        const double current_seconds {internal::Window::get_time()};
        const double elapsed_seconds {current_seconds - frame_counter.previous_seconds};
        frame_counter.previous_seconds = current_seconds;

        frame_counter.total_time += elapsed_seconds;

        if (frame_counter.total_time > 0.25) {
            ctx.fps = static_cast<float>(static_cast<double>(frame_counter.frame_count) / frame_counter.total_time);
            frame_counter.frame_count = 0;
            frame_counter.total_time = 0.0;
        }
        frame_counter.frame_count++;

        const double delta_time {std::min(elapsed_seconds, MAX_DT)};

        return static_cast<float>(delta_time);
    }

    unsigned int Application::calculate_fixed_update() {
        static constexpr double FIXED_DT {1.0 / 50.0};

        const double current_seconds {internal::Window::get_time()};
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
            LOG_INFO("Changing scene to {}...", next_scene->get_name());

            ctx.rnd.post_setup();

            LOG_INFO("Calling scene on_stop...");

            current_scene->on_stop();

            LOG_INFO("Clearing resources cache...");

            ctx.res.clear();
            ctx.evt.disconnect(current_scene);
            current_scene = std::exchange(next_scene, nullptr);

            LOG_INFO("Calling scene on_start...");

            current_scene->on_start();
            ctx.rnd.pre_setup();
        }

        assert(next_scene == nullptr);
    }

    void Application::dear_imgui_render() {
        internal::imgui_context::begin_frame();

        current_scene->on_imgui_update();
        ctx.dbg.render_dear_imgui(ctx.scn, ctx);

        internal::imgui_context::end_frame();
    }

    void Application::setup_start_scene(Id start_scene_id) {
        for (const std::unique_ptr<ApplicationScene>& scene : scenes) {
            if (scene->id == start_scene_id) {
                current_scene = scene.get();
            }
        }

        assert(current_scene != nullptr);
    }

    void Application::on_window_closed(const WindowClosedEvent&) {
        ctx.running = false;
    }

    void Application::on_window_resized(const WindowResizedEvent& event) {
        ctx.rnd.resize_framebuffers(event.width, event.height);
    }

    void Application::on_window_iconified(const WindowIconifiedEvent& event) {
        minimized = event.iconified;
    }
}
