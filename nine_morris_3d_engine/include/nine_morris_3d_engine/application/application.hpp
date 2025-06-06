#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <utility>

#include "nine_morris_3d_engine/application/properties.hpp"
#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/events.hpp"
#include "nine_morris_3d_engine/application/id.hpp"

namespace sm {
    class ApplicationScene;
    class Window;

    // Application initialization/uninitialization callbacks
    struct UserFunctions {
        // Not called when an exception is thrown
        std::function<void(Ctx&)> start {[](Ctx&) {}};
        std::function<void(Ctx&)> stop {[](Ctx&) {}};
    };

    // Main class storing all other components
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
            MetaScene meta_scene;
            meta_scene.id = Id(S::get_static_name());
            meta_scene.constructor = [this]() { return std::make_unique<S>(m_ctx); };

            m_scene_meta_scenes.push_back(std::move(meta_scene));
        }

        // Setup a struct that is shared across all scenes
        // The lifetime of it is from this function call until the destruction of the application class
        template<typename T>
        void set_global_data() {
            m_ctx.m_global_data = std::make_unique<T>();
        }
    private:
        struct MetaScene {
            Id id;
            std::function<std::unique_ptr<ApplicationScene>()> constructor;
            std::unique_ptr<ApplicationScene> scene;
        };

        float update_frame_counter();
        unsigned int calculate_fixed_update();
        void update_projection_matrices(int width, int height);
        void check_changed_scene();
        void change_scene(Id id, bool clear_resources = false);
        void dear_imgui_render();
        void setup_start_scene(Id start_scene_id);
        void scene_on_start(MetaScene* meta_scene);
        void scene_on_stop(MetaScene* meta_scene);

        void on_window_closed(const WindowClosedEvent& event);
        void on_window_resized(const WindowResizedEvent& event);
        void on_window_restored(const WindowRestoredEvent& event);
        void on_window_minimized(const WindowMinimizedEvent& event);

        // API accessible to the user
        Ctx m_ctx;

        std::vector<MetaScene> m_scene_meta_scenes;
        MetaScene* m_scene_current {};
        MetaScene* m_scene_next {};
        bool m_scene_clear_resources {};

        // Keep track of window state to skip rendering
        bool m_minimized {false};

        // Clock variables
        struct {
            double previous_seconds {};
            double total_time {};
            int frame_count {};
        } m_frame_counter;

        struct {
            double previous_seconds {};
            double total_time {};
        } m_fixed_update;

        friend class ApplicationScene;
        friend class Window;
        friend class Ctx;
    };
}
