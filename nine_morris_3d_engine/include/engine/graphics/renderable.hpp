#pragma once

#include <memory>
#include <variant>

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/material.hpp"
#include "engine/graphics/font.hpp"

namespace sm {
    // Right now every renderable is analogous to a mesh, to a render call
    struct Renderable {
        struct Transform {
            // Don't default initialize
            glm::vec3 position;
            glm::vec3 rotation;
            float scale;
        };

        void position(const glm::vec3& position) {
            std::get<0>(transform).position = position;
        }

        void rotation(const glm::vec3& rotation) {
            std::get<0>(transform).rotation = rotation;
        }

        void scale(float scale) {
            std::get<0>(transform).scale = scale;
        }

        std::weak_ptr<GlVertexArray> vertex_array;
        std::weak_ptr<MaterialInstance> material;

        std::variant<Transform, glm::mat4> transform;

        glm::vec3 outline_color {};  // TODO think of a good solution
    };

    // Every piece of text is a render call too
    struct Text {
        std::weak_ptr<Font> font;

        std::string text;
        glm::vec2 position {};
        glm::vec3 color {};
        float scale {1.0f};
    };
}
