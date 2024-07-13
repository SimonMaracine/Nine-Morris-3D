#pragma once

#include <memory>
#include <variant>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/material.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    class Renderer;
    class DebugUi;

    class Renderable {
    public:
        Renderable() = default;
        Renderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<GlVertexArray> vertex_array, std::shared_ptr<MaterialInstance> material)
            : mesh(mesh), vertex_array(vertex_array), material(material), aabb(mesh->get_aabb()) {}

        const utils::AABB& get_aabb() const { return aabb; }
        MaterialInstance* get_material() const { return material.get(); }

        struct {
            glm::vec3 position {};
            glm::vec3 rotation {};
            float scale {1.0f};  // Only uniform scaling
        } transform;

        struct {
            glm::vec3 color {1.0f};  // TODO think of a good solution
            float thickness {0.1f};
        } outline;
    private:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<GlVertexArray> vertex_array;
        std::shared_ptr<MaterialInstance> material;

        utils::AABB aabb;  // FIXME think of a good solution

        friend class Renderer;
        friend class DebugUi;
    };

    struct Text {
        std::shared_ptr<Font> font;
        std::string text;
        glm::vec2 position {};
        glm::vec3 color {};
        float scale {1.0f};
    };

    struct Quad {
        std::shared_ptr<GlTexture> texture;
        glm::vec2 position {};
        glm::vec2 scale {1.0f};
    };

    struct DebugLine {
        glm::vec3 position1 {};
        glm::vec3 position2 {};
        glm::vec3 color {};
    };
}
