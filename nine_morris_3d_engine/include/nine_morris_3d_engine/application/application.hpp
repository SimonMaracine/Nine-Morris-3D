#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "nine_morris_3d_engine/application/scene.hpp"
#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/id.hpp"

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
            scene.meta_scenes.push_back({
                Id(S::static_name()),
                [this]() {
                    return std::make_unique<S>(ctx);
                },
                nullptr
            });
        }

        // Setup a struct that is shared across all scenes
        template<typename T>
        void set_global_data() {
            ctx.global_data.emplace<T>();
        }

        // API accessible to the user
        Ctx ctx;
    private:
        struct MetaScene {
            Id id;
            std::function<std::unique_ptr<ApplicationScene>()> constructor;
            std::unique_ptr<ApplicationScene> scene;
        };

        float update_frame_counter();
        unsigned int calculate_fixed_update();
        void check_changed_scene();
        void dear_imgui_render();
        void setup_start_scene(Id start_scene_id);
        void scene_on_start(MetaScene* meta_scene);
        void scene_on_stop(MetaScene* meta_scene);

        void on_window_closed(const WindowClosedEvent&);
        void on_window_resized(const WindowResizedEvent& event);
        void on_window_iconified(const WindowIconifiedEvent& event);

        struct {
            std::vector<MetaScene> meta_scenes;
            MetaScene* current {nullptr};
            MetaScene* next {nullptr};
        } scene;

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
