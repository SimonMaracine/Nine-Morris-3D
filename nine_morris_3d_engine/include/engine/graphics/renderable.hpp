#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/material.hpp"

namespace sm {
    // Right now every renderable is analogous to a mesh, to a render call
    struct Renderable {
        std::weak_ptr<GlVertexArray> vertex_array;
        std::weak_ptr<MaterialInstance> material;

        glm::vec3 position {};
        glm::vec3 rotation {};
        float scale {1.0f};

        glm::vec3 outline_color {};  // TODO think of a good solution
    };
}
