#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/graphics/internal/scene.hpp"

namespace sm {
    class Ctx;
}

namespace sm::internal {
#ifndef SM_BUILD_DISTRIBUTION
    class DebugUi {
    public:
        void render(Scene& scene, Ctx& ctx) noexcept;
        void render_lines(Scene& scene);
    private:
        void renderables(Scene& scene) noexcept;
        void lights(Scene& scene) noexcept;
        void shadows(Scene& scene) noexcept;
        void texts(Scene& scene) noexcept;
        void quads(Scene& scene) noexcept;
        void tasks(Ctx& ctx) noexcept;

        void shadows_lines(
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

        bool m_vsync {true};
        bool m_renderables {false};
        bool m_lights {false};
        bool m_shadows {false};
        bool m_texts {false};
        bool m_quads {false};
        bool m_tasks {false};
    };
#else
    class DebugUi {};
#endif
}
