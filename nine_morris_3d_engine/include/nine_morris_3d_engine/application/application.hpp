#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/scene.hpp"
#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/audio/context.hpp"

namespace sm {
    class Window;

    struct UserFunctions {
        // Not called when an exception is thrown
        std::function<void(Ctx&)> start {[](Ctx&) {}};
        std::function<void(Ctx&)> stop {[](Ctx&) {}};
    };

    class Application final {
    public:
        explicit Application(const ApplicationProperties& properties);
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        // Launch the application; return an exit code
        int run(Id start_scene_id, const UserFunctions& user_functions = {});

        // Add scenes to the application before calling run()
        template<typename S>
        void add_scene() {
            scenes.push_back(std::make_unique<S>(ctx));
        }

        // Setup a struct that is shared across all scenes
        template<typename T>
        void set_global_data() {
            ctx.global_data.emplace<T>();
        }

        // API accessible to the user
        Ctx ctx;
    private:
        float update_frame_counter();
        unsigned int calculate_fixed_update();
        void check_changed_scene();
        void dear_imgui_render();
        void setup_start_scene(Id start_scene_id);

        void on_window_closed(const WindowClosedEvent&);
        void on_window_resized(const WindowResizedEvent& event);
        void on_window_iconified(const WindowIconifiedEvent& event);

        // Scene system
        std::vector<std::unique_ptr<ApplicationScene>> scenes;
        ApplicationScene* current_scene {nullptr};
        ApplicationScene* next_scene {nullptr};

        // Clock variables
        struct {
            double previous_seconds {};
            double total_time {};
            int frame_count {};
        } frame_counter;

        struct {
            double previous_seconds {};
            double total_time {};
        } fixed_update;

        // Keep track of window state to skip rendering
        bool minimized {false};

        friend class ApplicationScene;
        friend class Window;
        friend class Ctx;
    };
}
