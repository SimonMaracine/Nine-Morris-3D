#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/graphics/internal/scene.hpp"

namespace sm {
    class Ctx;
}

namespace sm::internal {
    class DebugUi {
    public:
        void render_dear_imgui(Scene& scene, Ctx& ctx) noexcept;
        void add_lines(Scene& scene);

        void draw_renderables(Scene& scene) noexcept;
        void draw_lights(Scene& scene) noexcept;
        void draw_shadows(Scene& scene) noexcept;
        void draw_texts(Scene& scene) noexcept;
        void draw_quads(Scene& scene) noexcept;
        void draw_tasks(Ctx& ctx) noexcept;

        void add_shadows_lines(
            Scene& scene,
            float left,
            float right,
            float bottom,
            float top,
            float near,
            float far,
            glm::vec3 position,
            glm::vec3 orientation
        );
    private:
        bool vsync {true};
        bool renderables {false};
        bool lights {false};
        bool shadows {false};
        bool texts {false};
        bool quads {false};
        bool tasks {false};
    };
}
