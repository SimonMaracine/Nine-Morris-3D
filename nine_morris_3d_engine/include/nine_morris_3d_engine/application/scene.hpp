#pragma once

#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/id.hpp"

namespace sm {
    // An entire part of a game, holding lots of state
    class ApplicationScene {
    public:
        explicit ApplicationScene(Ctx& ctx)
            : ctx(ctx) {}
        virtual ~ApplicationScene() = default;

        ApplicationScene(const ApplicationScene&) = delete;
        ApplicationScene& operator=(const ApplicationScene&) = delete;
        ApplicationScene(ApplicationScene&&) = delete;
        ApplicationScene& operator=(ApplicationScene&&) = delete;

        virtual const char* get_name() const = 0;  // Get the name of the scene
        virtual void on_start() {}  // Called when the scene is entered
        virtual void on_stop() {}  // Called when the scene is exited, except when an exception is thrown
        virtual void on_update() {}  // Called every frame
        virtual void on_fixed_update() {}  // Called once every 50 milliseconds
        virtual void on_imgui_update() {}  // Called every frame for Dear ImGui only

        // Retrieve the scene ID
        Id get_id() const {
            return Id(get_name());
        }

        // Window to the user API
        Ctx& ctx;
    private:
        void pre_update();
        void post_update();

        friend class Application;
    };
}

// Used inside a scene class definition to autocomplete name functions
#define SM_SCENE_NAME(NAME) \
    static const char* get_static_name() { return NAME; } \
    const char* get_name() const override { return get_static_name(); }
