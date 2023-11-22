#pragma once

#include <glm/glm.hpp>

#include "engine/other/assert.hpp"

namespace sm {
    class Camera;
    class Ctx;

    class CameraController {
    public:
        CameraController() = default;
        CameraController(Camera* camera, Ctx* ctx)
            : camera(camera), ctx(ctx) {}
        virtual ~CameraController() = default;

        CameraController(const CameraController&) = default;
        CameraController& operator=(const CameraController&) = default;
        CameraController(CameraController&&) noexcept = default;
        CameraController& operator=(CameraController&&) noexcept = default;

        virtual void update_controls(float dt) = 0;
        virtual void update_camera(float dt) = 0;
        virtual glm::vec3 get_position() const = 0;
        virtual glm::vec3 get_rotation() const = 0;

        const Camera* get_camera() const {
            return camera;
        }

        void set_camera(Camera* camera) {
            SM_ASSERT(camera != nullptr, "Camera must not be null");
            this->camera = camera;
        }
    protected:
        Camera* camera {nullptr};
        Ctx* ctx {nullptr};
    };
}
