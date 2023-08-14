#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/material.hpp"

namespace sm {
    struct Renderable {
        std::shared_ptr<GlVertexArray> vertex_array;
        std::shared_ptr<GlIndexBuffer> index_buffer;
        std::shared_ptr<MaterialInstance> material;

        glm::vec3 position {};
        glm::vec3 rotation {};
        float scale = 1.0f;

        glm::vec3 outline_color {};
    };
}
