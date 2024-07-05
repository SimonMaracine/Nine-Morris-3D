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

        void set_position(glm::vec3 position) { transform.position = position; }
        void set_rotation(glm::vec3 rotation) { transform.rotation = rotation; }
        void set_scale(float scale) { transform.scale = scale; }
        void set_x(float x) { transform.position.x = x; }
        void set_y(float y) { transform.position.y = y; }
        void set_z(float z) { transform.position.z = z; }
        void set_rx(float rx) { transform.rotation.x = rx; }
        void set_ry(float ry) { transform.rotation.y = ry; }
        void set_rz(float rz) { transform.rotation.z = rz; }

        glm::vec3 get_position() const { return transform.position; }
        glm::vec3 get_rotation() const { return transform.rotation; }
        float get_scale() const { return transform.scale; }
        const utils::AABB& get_aabb() const { return aabb; }
        MaterialInstance* get_material() const { return material.get(); }
    private:
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<GlVertexArray> vertex_array;
        std::shared_ptr<MaterialInstance> material;

        struct {
            glm::vec3 position {};
            glm::vec3 rotation {};
            float scale {1.0f};  // Only uniform scaling
        } transform;

        struct {
            glm::vec3 color {};  // TODO think of a good solution
            glm::vec3 offset {};
            float scale {1.05f};
        } outline;

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
        glm::vec2 position {};
        glm::vec2 scale {1.0f};
        std::shared_ptr<GlTexture> texture;
    };

    struct DebugLine {
        glm::vec3 position1 {};
        glm::vec3 position2 {};
        glm::vec3 color {};
    };
}
