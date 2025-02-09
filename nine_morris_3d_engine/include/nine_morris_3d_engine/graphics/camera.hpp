#pragma once

#include <glm/glm.hpp>

// Object representing a cameras in 3D and 2D, matrices only

namespace sm {
    class Camera3D {
    public:
        void set_position_orientation(glm::vec3 position, glm::vec3 at, glm::vec3 up);
        void set_position_rotation(glm::vec3 position, glm::vec3 rotation);

        void set_projection(int width, int height, float fov, float near, float far);
        void set_view(const glm::mat4& view_matrix);

        const glm::mat4& view() const;
        const glm::mat4& projection() const;
        const glm::mat4& projection_view() const;
    private:
        glm::mat4 m_view_matrix {1.0f};
        glm::mat4 m_projection_matrix {1.0f};

        // This is a cache
        glm::mat4 m_projection_view_matrix {1.0f};
    };

    class Camera2D {
    public:
        void set_projection(int left, int right, int bottom, int top);

        const glm::mat4& projection() const;
    private:
        glm::mat4 m_projection_matrix {1.0f};
    };
}
