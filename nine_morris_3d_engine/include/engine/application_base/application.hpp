#pragma once

#include <string_view>
#include <string>
#include <functional>
#include <memory>
#include <vector>

#include "engine/application_base/application_properties.hpp"
#include "engine/application_base/events.hpp"
#include "engine/application_base/window.hpp"
#include "engine/application_base/application_builder.hpp"
#include "engine/application_base/context.hpp"
#include "engine/audio/context.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"  // TODO remove
#include "engine/graphics/renderer/renderer.hpp"
#include "engine/graphics/renderer/gui_renderer.hpp"
#include "engine/other/resource_manager.hpp"
#include "engine/scene/scene.hpp"

namespace sm {
    class Application final {
    public:
        using SceneId = Scene::SceneId;
        using UserFunc = std::function<void(Ctx*)>;
        using RendererFunc = std::function<void()>;

        struct ApplicationsData {
            std::string app_name;
            std::string log_file;
            std::string info_file;
            std::string res_directory;
        };

        static void initialize_applications(const ApplicationsData& data);

        Application(const ApplicationBuilder& builder, void* user_data = nullptr);
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        // Call this to launch the application; it can return an exit code
        int run(SceneId start_scene_id);

        // Add scenes to the application before calling run()
        template<typename S>
        void add_scene() {
            scenes.push_back(std::make_unique<S>());
        }

        void set_start_function(const UserFunc& start);  // TODO change
        void set_stop_function(const UserFunc& stop);

        // API accessible to the user
        Ctx ctx;
    private:
        float update_frame_counter();
        unsigned int calculate_fixed_update();
        void check_changed_scene();

        void r2d_function();
        void dear_imgui_function();

        void prepare_scenes(SceneId start_scene_id);
        void on_start(Scene* scene);
        void user_start_function();
        void user_stop_function();
        void initialize_r3d();
        void initialize_r2d();
        void initialize_dear_imgui();
        void initialize_audio();
        void initialize_random_generator();

        void on_window_closed(const WindowClosedEvent&);
        void on_window_resized(const WindowResizedEvent& event);

        ApplicationProperties properties;
        UserFunc start = [](Ctx*) {};
        UserFunc stop = [](Ctx*) {};
        bool with_dear_imgui = false;

        // Data for the scene system
        std::vector<std::unique_ptr<Scene>> scenes;
        Scene* current_scene = nullptr;

        bool changed_scene = false;  // Flag set when the user requested a scene change
        Scene* to_scene = nullptr;  // Next scene to enter

        // Data for modular rendering
        RendererFunc r3d_update = []() {};
        RendererFunc r2d_update = []() {};
        RendererFunc dear_imgui_update = []() {};

        // Keep track of all framebuffers to resize them, if needed
        std::vector<std::weak_ptr<GlFramebuffer>> framebuffers;

        // Clock variables
        struct {
            double previous_seconds = 0.0;
            double total_time = 0.0;
            int frame_count = 0;
        } frame_counter;

        struct {
            double previous_seconds = 0.0;
            double total_time = 0.0;
        } fixed_update;

        friend class Scene;
        friend class Window;
        friend class Ctx;
    };
}
