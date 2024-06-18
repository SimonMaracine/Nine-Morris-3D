#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>

#include "engine/application_base/properties.hpp"
#include "engine/application_base/events.hpp"
#include "engine/application_base/window.hpp"
#include "engine/application_base/context.hpp"
#include "engine/application_base/id.hpp"
#include "engine/audio/context.hpp"
#include "engine/graphics/renderer.hpp"
#include "engine/other/resources_cache.hpp"
#include "engine/scene/scene.hpp"

namespace sm {
    struct ApplicationsData {
        std::string app_name;
        std::string log_file;
        std::string info_file;
        std::string res_directory;
    };

    struct UserFunctions {
        std::function<void(Ctx&)> start {[](Ctx&) {}};
        std::function<void(Ctx&)> stop {[](Ctx&) {}};
    };

    class Application final {
    public:
        static bool initialize_applications(const ApplicationsData& data);

        explicit Application(const ApplicationProperties& properties);
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        // Call this to launch the application; it can return an exit code
        int run(Id start_scene_id, const UserFunctions& user_functions = {});

        // Add scenes to the application before calling run()
        template<typename S>
        void add_scene() {
            scenes.push_back(std::make_unique<S>());
        }

        // API accessible to the user
        Ctx ctx;
    private:
        float update_frame_counter();
        unsigned int calculate_fixed_update();
        void check_changed_scene();
        void dear_imgui_render();
        void prepare_scenes(Id start_scene_id);

        void on_window_closed(const WindowClosedEvent&);
        void on_window_resized(const WindowResizedEvent& event);
        void on_window_iconified(const WindowIconifiedEvent& event);

        // Data for the scene system
        std::vector<std::unique_ptr<Scene>> scenes;
        Scene* current_scene {nullptr};
        Scene* next_scene {nullptr};  // Next scene to enter

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

        friend class Scene;
        friend class Window;
        friend class Ctx;
    };
}
