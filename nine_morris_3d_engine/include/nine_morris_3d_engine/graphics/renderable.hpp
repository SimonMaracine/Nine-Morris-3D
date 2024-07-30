#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/material.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"

namespace sm {
    namespace internal {
        class Renderer;
        class DebugUi;
    }

    class Renderable {
    public:
        Renderable() noexcept = default;
        Renderable(
            std::shared_ptr<Mesh> mesh,
            std::shared_ptr<GlVertexArray> vertex_array,
            std::shared_ptr<MaterialInstance> material
        ) noexcept
            : m_mesh(mesh), m_vertex_array(vertex_array), m_material(material) {}

        const utils::AABB& get_aabb() const noexcept { return m_mesh->get_aabb(); }
        MaterialInstance* get_material() const noexcept { return m_material.get(); }
        operator bool() const noexcept { return m_mesh && m_vertex_array && m_material; }

        struct Transform {
            glm::vec3 position {};
            glm::vec3 rotation {};
            float scale {1.0f};  // Only uniform scaling
        } transform;

        struct {
            glm::vec3 color {1.0f};
            float thickness {0.1f};
        } outline;
    private:
        std::shared_ptr<Mesh> m_mesh;
        std::shared_ptr<GlVertexArray> m_vertex_array;
        std::shared_ptr<MaterialInstance> m_material;

        friend class internal::Renderer;
        friend class internal::DebugUi;
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
