#pragma once

#include <glm/glm.hpp>

namespace sm {
    class Camera {
    public:
        void set_position_orientation(glm::vec3 position, glm::vec3 at, glm::vec3 up) noexcept;
        void set_position_rotation(glm::vec3 position, glm::vec3 rotation) noexcept;

        void set_projection(int width, int height, float fov, float near, float far) noexcept;
        void set_view(const glm::mat4& view_matrix) noexcept;

        const glm::mat4& view() const noexcept;
        const glm::mat4& projection() const noexcept;
        const glm::mat4& projection_view() const noexcept;
    private:
        glm::mat4 m_view_matrix {1.0f};
        glm::mat4 m_projection_matrix {1.0f};

        // This is a cache
        glm::mat4 m_projection_view_matrix {1.0f};
    };

    class Camera2D {
    public:
        void set_projection(int left, int right, int bottom, int top) noexcept;

        const glm::mat4& projection() const noexcept;
    private:
        glm::mat4 m_projection_matrix {1.0f};
    };
}
