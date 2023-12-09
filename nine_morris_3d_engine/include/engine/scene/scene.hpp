#pragma once

#include <string>

#include <resmanager/resmanager.hpp>

namespace sm {
    class Ctx;
    class Application;

    /*
        Class representing an entire scene of a game. Subclasses need to be heap-allocated.
    */
    class Scene {
    public:
        using SceneId = resmanager::HashedStr64;

        Scene(const std::string& name)
            : name(name), id(SceneId(name)) {}
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
        SceneId get_id() const { return id; }

        Ctx* ctx {nullptr};
    private:
        std::string name;
        SceneId id;

        friend class Ctx;
        friend class Application;
    };
}
