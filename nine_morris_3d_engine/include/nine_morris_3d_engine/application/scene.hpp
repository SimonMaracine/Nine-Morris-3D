#pragma once

#include <string>

#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/id.hpp"

namespace sm {
    class Application;

    // An entire part of a game, holding lots of state
    class Scene {
    public:
        Scene(Ctx& ctx, const std::string& name)
            : ctx(ctx), name(name), id(Id(name)) {}
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        virtual void on_start() {}  // Called when the scene is entered
        virtual void on_stop() {}  // Called when the scene is exited
        virtual void on_update() {}  // Called every frame
        virtual void on_fixed_update() {}  // Called once every 50 milliseconds
        virtual void on_imgui_update() {}  // Called every frame for Dear ImGui only

        const std::string& get_name() const { return name; }
        Id get_id() const { return id; }

        Ctx& ctx;
    private:
        std::string name;
        Id id;

        friend class Application;
        friend class Ctx;
    };
}
