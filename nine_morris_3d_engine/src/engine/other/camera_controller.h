#pragma once

#include <glm/glm.hpp>

#include "engine/graphics/camera.h"

class CameraController {
public:
    CameraController() = default;
    CameraController(Camera* camera)
        : camera(camera) {}
    virtual ~CameraController() = default;

    virtual void update_controls(float dt) = 0;
    virtual void update_camera(float dt) = 0;
    virtual const glm::vec3& get_position() const = 0;
    virtual const glm::vec3& get_rotation() const = 0;

    const Camera& get_camera() const { return *camera; }
    void set_camera(Camera* camera) { this->camera = camera; }
protected:
    Camera* camera = nullptr;
};
