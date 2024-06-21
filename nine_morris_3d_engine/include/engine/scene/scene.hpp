#pragma once

#include <string>

#include "engine/application_base/context.hpp"
#include "engine/application_base/id.hpp"

namespace sm {
    class Application;

    // An entire scene of a game
    class Scene {
    public:
        explicit Scene(const std::string& name)
            : name(name), id(Id(name)) {}
        virtual ~Scene() = default;

        Scene(const Scene&) = default;
        Scene& operator=(const Scene&) = default;
        Scene(Scene&&) = default;
        Scene& operator=(Scene&&) = default;

        virtual void on_start() {}  // Called when the scene is entered
        virtual void on_stop() {}  // Called when the scene is exited
        virtual void on_update() {}  // Called every frame
        virtual void on_fixed_update() {}  // Called once every 50 milliseconds
        virtual void on_imgui_update() {}  // Called every frame for Dear ImGui only

        const std::string& get_name() const { return name; }
        Id get_id() const { return id; }

        Ctx* ctx {nullptr};
    private:
        std::string name;
        Id id;

        friend class Application;
        friend class Ctx;
    };
}
