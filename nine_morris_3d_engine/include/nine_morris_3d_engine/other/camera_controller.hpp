#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/graphics/camera.hpp"

namespace sm {
    class CameraController {
    public:
        CameraController() = default;
        explicit CameraController(Camera& camera)
            : camera(&camera) {}
        virtual ~CameraController() = default;

        CameraController(const CameraController&) = default;
        CameraController& operator=(const CameraController&) = default;
        CameraController(CameraController&&) = default;
        CameraController& operator=(CameraController&&) = default;

        virtual void update_controls(float dt, const Ctx& ctx) = 0;
        virtual void update_camera(float dt) = 0;
        virtual glm::vec3 get_position() const = 0;
        virtual glm::vec3 get_rotation() const = 0;

        const Camera& get_camera() const {
            return *camera;
        }

        void set_camera(Camera& camera) {
            this->camera = &camera;
        }
    protected:
        Camera* camera {};
    };
}
