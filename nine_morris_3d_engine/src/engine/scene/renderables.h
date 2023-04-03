#pragma once

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/opengl/texture.h"
#include "engine/graphics/material.h"
#include "engine/graphics/identifier.h"
#include "engine/scene/object.h"  // TODO not great

namespace renderables {
    struct Model : public Object {
        struct BoundingBox {
            Identifier::Id id;
            glm::vec3 size = glm::vec3(0.0f);
            bool sort = true;
        };

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        float scale = 1.0f;

        std::shared_ptr<gl::VertexArray> vertex_array;
        std::shared_ptr<gl::IndexBuffer> index_buffer;
        std::shared_ptr<MaterialInstance> material;

        std::optional<glm::vec3> outline_color;
        std::optional<BoundingBox> bounding_box;
        bool cast_shadow = false;
    };

    struct Quad : public Object {
        glm::vec3 position = glm::vec3(0.0f);
        float scale = 1.0f;

        std::shared_ptr<gl::Texture> texture;
    };
}
