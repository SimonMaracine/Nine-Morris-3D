#pragma once

#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/id.hpp"

namespace sm {
    // An entire part of a game, holding lots of state
    class ApplicationScene {
    public:
        explicit ApplicationScene(Ctx& ctx) noexcept
            : ctx(ctx) {}
        virtual ~ApplicationScene() noexcept = default;

        ApplicationScene(const ApplicationScene&) = delete;
        ApplicationScene& operator=(const ApplicationScene&) = delete;
        ApplicationScene(ApplicationScene&&) = delete;
        ApplicationScene& operator=(ApplicationScene&&) = delete;

        virtual const char* get_name() const noexcept = 0;
        virtual void on_start() {}  // Called when the scene is entered
        virtual void on_stop() {}  // Called when the scene is exited, except when an exception is thrown
        virtual void on_update() {}  // Called every frame
        virtual void on_fixed_update() {}  // Called once every 50 milliseconds
        virtual void on_imgui_update() {}  // Called every frame for Dear ImGui only

        Id get_id() const noexcept {
            return Id(get_name());
        }

        Ctx& ctx;
    };
}

#define SM_SCENE_NAME(NAME) \
    static const char* get_static_name() noexcept { return NAME; } \
    const char* get_name() const noexcept override { return get_static_name(); }
