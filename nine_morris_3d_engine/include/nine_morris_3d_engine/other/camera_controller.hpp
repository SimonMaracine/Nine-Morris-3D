#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/graphics/camera.hpp"

namespace sm {
    class Ctx;

    // Base class for a camera controller, used by the application
    // It references a 3D camera, controlling it
    class CameraController {
    public:
        CameraController() = default;
        explicit CameraController(Camera3D& camera)
            : m_camera(&camera) {}
        virtual ~CameraController() = default;

        CameraController(const CameraController&) = default;
        CameraController& operator=(const CameraController&) = default;
        CameraController(CameraController&&) = default;
        CameraController& operator=(CameraController&&) = default;

        virtual void update_controls(float dt, const Ctx& ctx) = 0;
        virtual void update_camera(float dt) = 0;

        virtual glm::vec3 get_position() const = 0;
        virtual glm::vec3 get_rotation() const = 0;

        virtual float get_fov() const = 0;
        virtual float get_near() const = 0;
        virtual float get_far() const = 0;

        const Camera3D& get_camera() const {
            return *m_camera;
        }

        void set_camera(Camera3D& camera) {
            m_camera = &camera;
        }
    protected:
        Camera3D* m_camera {};
    };
}
