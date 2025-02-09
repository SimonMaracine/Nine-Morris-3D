#pragma once

#include "nine_morris_3d_engine/other/camera_controller.hpp"

namespace sm::internal {
    static constexpr float LENS_FOV {45.0f};
    static constexpr float LENS_NEAR {0.5f};
    static constexpr float LENS_FAR {1000.0f};

    // The application has to have a camera controller, if one is not set by the client
    struct DefaultCameraController : CameraController {
        void update_controls(float, const Ctx&) override {}

        void update_camera(float) override {}

        glm::vec3 get_position() const override {
            return {};
        }

        glm::vec3 get_rotation() const override {
            return {};
        }

        float get_fov() const override {
            return LENS_FOV;
        }

        float get_near() const override {
            return LENS_NEAR;
        }

        float get_far() const override {
            return LENS_FAR;
        }
    };
}
