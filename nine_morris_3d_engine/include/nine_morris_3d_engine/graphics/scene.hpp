#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <string>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/material.hpp"
#include "nine_morris_3d_engine/graphics/mesh.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"
#include "nine_morris_3d_engine/graphics/light.hpp"
#include "nine_morris_3d_engine/graphics/camera.hpp"
#include "nine_morris_3d_engine/graphics/post_processing_step.hpp"
#include "nine_morris_3d_engine/graphics/skybox.hpp"
#include "nine_morris_3d_engine/other/camera_controller.hpp"
#include "nine_morris_3d_engine/application/id.hpp"

namespace sm {
    class ApplicationScene;

    namespace internal {
        class Renderer;
        class DebugUi;
    }

    enum NodeFlag {
        Inherited,  // The parent's or default value
        Enabled,
        Disabled
    };

    // Used to draw 3D debug lines
    struct DebugLine {
        glm::vec3 position1 {};
        glm::vec3 position2 {};
        glm::vec3 color {};
    };

    struct Context3D {
        glm::mat4 transform {1.0f};
        float transform_scale {1.0f};
        bool outline {false};
        bool disable_back_face_culling {false};
        bool cast_shadow {false};
    };

    struct Context2D {
        glm::vec2 position {};
        glm::vec2 scale {1.0f};
    };

    enum class SceneNode3DType {
        Root3D,
        Model,
        PointLight
    };

    enum class SceneNode2DType {
        Root2D,
        Image,
        Text
    };

    // Base class for a 3D scene node
    class SceneNode3D : public std::enable_shared_from_this<SceneNode3D> {
    public:
        SceneNode3D() = default;
        explicit SceneNode3D(Id id)
            : id(id) {}
        virtual ~SceneNode3D() = default;

        // Get the node type
        virtual SceneNode3DType type() const = 0;

        // Attach a node as a child
        void add_node(std::shared_ptr<SceneNode3D> node);

        // Recursively clear the nodes
        void clear_nodes();

        // Traverse the tree
        void traverse(const std::function<bool(std::shared_ptr<SceneNode3D>)>& process);
        void traverse(const std::function<bool(SceneNode3D*)>& process);
        void traverse(const std::function<bool(const SceneNode3D*, Context3D&)>& process) const;

        // Find a node by identifier
        std::shared_ptr<SceneNode3D> find_node(Id id);

        // Generic traverse
        template<typename T>
        void traverse(T context, const std::function<bool(const SceneNode3D*, T&)>& process) const {
            if (process(this, context)) {
                return;
            }

            for (const auto& child : m_children) {
                child->traverse(context, process);
            }
        }

        // Every node has an optional identifier (tag)
        Id id;
    protected:
        std::vector<std::shared_ptr<SceneNode3D>> m_children;
        std::weak_ptr<SceneNode3D> m_parent;
    };

    // Base class for a 2D scene node
    class SceneNode2D : public std::enable_shared_from_this<SceneNode2D> {
    public:
        SceneNode2D() = default;
        explicit SceneNode2D(Id id)
            : id(id) {}
        virtual ~SceneNode2D() = default;

        // Get the node type
        virtual SceneNode2DType type() const = 0;

        // Attach a node as a child
        void add_node(std::shared_ptr<SceneNode2D> node);

        // Recursively clear the nodes
        void clear_nodes();

        // Traverse the tree
        void traverse(const std::function<bool(std::shared_ptr<SceneNode2D>)>& process);
        void traverse(const std::function<bool(SceneNode2D*)>& process);
        void traverse(const std::function<bool(const SceneNode2D*, Context2D&)>& process) const;

        // Find a node by identifier
        std::shared_ptr<SceneNode2D> find_node(Id id);

        // Generic traverse
        template<typename T>
        void traverse(T context, const std::function<bool(const SceneNode2D*, T&)>& process) const {
            if (process(this, context)) {
                return;
            }

            for (const auto& child : m_children) {
                child->traverse(context, process);
            }
        }

        // Every node has an optional identifier (tag)
        Id id;
    protected:
        std::vector<std::shared_ptr<SceneNode2D>> m_children;
        std::weak_ptr<SceneNode2D> m_parent;
    };

    // Root node contains general information about the 3D scene
    class RootNode3D : public SceneNode3D {
    public:
        SceneNode3DType type() const override {
            return SceneNode3DType::Root3D;
        }

        glm::vec3 get_camera_position() const { return m_camera_position; }

        // Immediate mode debug API
        void debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color);
        void debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color);
        void debug_add_point(glm::vec3 position, glm::vec3 color);
        void debug_add_lamp(glm::vec3 position, glm::vec3 color);
        void debug_clear();

        Camera3D camera;
        Skybox skybox;
        DirectionalLight directional_light;
        ShadowBox shadow_box;
        std::shared_ptr<CameraController> camera_controller;
    private:
        void update_shadow_box();
        void update_camera();

        glm::vec3 m_camera_position {};
        std::vector<std::shared_ptr<PostProcessingStep>> m_post_processing_steps;

#ifndef SM_BUILD_DISTRIBUTION
        std::vector<DebugLine> m_debug_lines;
#endif

        friend class ApplicationScene;
        friend class internal::Renderer;
        friend class internal::DebugUi;
    };

    class ModelNode : public SceneNode3D {
    public:
        ModelNode(std::shared_ptr<Mesh> mesh, std::shared_ptr<GlVertexArray> vertex_array, std::shared_ptr<MaterialInstance> material)
            : m_mesh(mesh), m_vertex_array(vertex_array), m_material(material) {}

        SceneNode3DType type() const override {
            return SceneNode3DType::Model;
        }

        const utils::AABB& get_aabb() const { return m_mesh->get_aabb(); }
        std::shared_ptr<MaterialInstance> get_material() const { return m_material; }

        glm::vec3 position {};
        glm::vec3 rotation {};
        float scale {1.0f};

        NodeFlag outline {Inherited};
        glm::vec3 outline_color {1.0f};
        float outline_thickness {1.1f};

        NodeFlag disable_back_face_culling {Inherited};
        NodeFlag cast_shadow {Inherited};
    private:
        std::shared_ptr<Mesh> m_mesh;
        std::shared_ptr<GlVertexArray> m_vertex_array;
        std::shared_ptr<MaterialInstance> m_material;

        friend class internal::Renderer;
    };

    struct PointLightNode : public SceneNode3D, public PointLight {  // Boo
        SceneNode3DType type() const override {
            return SceneNode3DType::PointLight;
        }
    };

    class RootNode2D : public SceneNode2D {
    public:
        SceneNode2DType type() const override {
            return SceneNode2DType::Root2D;
        }

        Camera2D camera;

        friend class internal::Renderer;
    };

    class ImageNode : public SceneNode2D {
    public:
        explicit ImageNode(std::shared_ptr<GlTexture> texture)
            : m_texture(texture) {}

        SceneNode2DType type() const override {
            return SceneNode2DType::Image;
        }

        void set_texture(std::shared_ptr<GlTexture> texture) { m_texture = texture; }
        std::shared_ptr<GlTexture> get_texture() const { return m_texture; }

        glm::vec2 position {};
        glm::vec2 scale {1.0f};
    private:
        std::shared_ptr<GlTexture> m_texture;

        friend class internal::Renderer;
    };

    class TextNode : public SceneNode2D {
    public:
        explicit TextNode(std::shared_ptr<Font> font)
            : m_font(font) {}

        SceneNode2DType type() const override {
            return SceneNode2DType::Text;
        }

        void set_font(std::shared_ptr<Font> font) { m_font = font; }
        std::shared_ptr<Font> get_font() const { return m_font; }

        glm::vec2 position {};
        float scale {1.0f};
        glm::vec3 color {};
        std::string text;
    private:
        std::shared_ptr<Font> m_font;

        friend class internal::Renderer;
    };

    // The root nodes of the scene
    // Handled by application or context
    struct Scene {
        std::shared_ptr<RootNode3D> root_node_3d;
        std::shared_ptr<RootNode2D> root_node_2d;
    };
}
