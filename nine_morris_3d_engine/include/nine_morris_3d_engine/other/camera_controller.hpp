#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/graphics/camera.hpp"

namespace sm {
    class CameraController {
    public:
        CameraController() noexcept = default;
        explicit CameraController(Camera& camera) noexcept
            : camera(&camera) {}
        virtual ~CameraController() noexcept = default;

        CameraController(const CameraController&) noexcept = default;
        CameraController& operator=(const CameraController&) noexcept = default;
        CameraController(CameraController&&) noexcept = default;
        CameraController& operator=(CameraController&&) noexcept = default;

        virtual void update_controls(float dt, const Ctx& ctx) = 0;
        virtual void update_camera(float dt) = 0;
        virtual glm::vec3 get_position() const noexcept = 0;
        virtual glm::vec3 get_rotation() const noexcept = 0;

        const Camera& get_camera() const noexcept {
            return *camera;
        }

        void set_camera(Camera& camera) noexcept {
            this->camera = &camera;
        }
    protected:
        Camera* camera {};
    };
}
