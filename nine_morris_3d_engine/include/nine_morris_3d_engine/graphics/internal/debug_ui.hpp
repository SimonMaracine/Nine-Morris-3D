#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/graphics/scene.hpp"

namespace sm {
    class Ctx;
}

namespace sm::internal {
#ifndef SM_BUILD_DISTRIBUTION
    class DebugUi {
    public:
        void render(const Scene& scene, Ctx& ctx);
        void render_lines(const Scene& scene);
    private:
        void models(const Scene& scene);
        void lights(const Scene& scene);
        void shadows(const Scene& scene);
        void images(const Scene& scene);
        void texts(const Scene& scene);
        void tasks(Ctx& ctx);
        void frame_time(Ctx& ctx);

        void shadows_lines(
            const Scene& scene,
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
        bool m_models {false};
        bool m_lights {false};
        bool m_shadows {false};
        bool m_images {false};
        bool m_texts {false};
        bool m_tasks {false};
        bool m_frame_time {false};

        std::vector<ModelNode*> m_model_nodes;
        std::vector<PointLightNode*> m_point_light_nodes;
        std::vector<ImageNode*> m_image_nodes;
        std::vector<TextNode*> m_text_nodes;

        static constexpr std::size_t FRAMES_SIZE {100};
        std::vector<float> m_frames {FRAMES_SIZE};
        std::size_t m_index {0};
    };
#else
    class DebugUi {};
#endif
}
