#pragma once

#include <glm/glm.hpp>

namespace sm {
    class Application;
    class Ctx;
    class Scene;

    class DebugUi {
    private:
        DebugUi() = default;

        void render_dear_imgui(Scene& scene);
        void add_lines(Scene& scene);

        void draw_renderables(Scene& scene);
        void draw_lights(Scene& scene);
        void draw_shadows(Scene& scene);
        void draw_texts(Scene& scene);
        void draw_quads(Scene& scene);

        void draw_shadows_lines(
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

        bool renderables {false};
        bool lights {false};
        bool shadows {false};
        bool texts {false};
        bool quads {false};

        friend class Application;
        friend class Ctx;
    };
}
