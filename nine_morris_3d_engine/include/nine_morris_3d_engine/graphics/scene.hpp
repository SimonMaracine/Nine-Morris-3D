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
// #include "nine_morris_3d_engine/graphics/post_processing_step.hpp"
#include "nine_morris_3d_engine/graphics/skybox.hpp"

namespace sm {
    namespace internal {
        class Renderer;
        class DebugRenderer;
        class DebugUi;
    }

//     class Scene {
//     public:
//         // Immediate
//         void capture(const Camera& camera, glm::vec3 position);
//         void capture(const Camera2D& camera_2d);
//         void environment(const Skybox& skybox);
//         void shadow(ShadowBox& box);
//         void add_post_processing(std::shared_ptr<PostProcessingStep> step);

//         // 3D API
//         void add_renderable(Renderable& renderable);
//         void add_light(DirectionalLight& light);
//         void add_light(PointLight& light);

//         // 2D API
//         void add_text(Text& text);
//         void add_quad(Quad& quad);

//         // Debug API
//         void debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color);
//         void debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color);
//         void debug_add_lines(std::initializer_list<glm::vec3> positions, glm::vec3 color);
//         void debug_add_point(glm::vec3 position, glm::vec3 color);
//         void debug_add_lamp(glm::vec3 position, glm::vec3 color);

//         void clear();

//         void shadow(const ShadowBox& box);
//         void add_renderable(const Renderable& renderable);
//         void add_light(const DirectionalLight& light);
//         void add_light(const PointLight& light);
//         void add_text(const Text& text);
//         void add_quad(const Quad& quad);
//     private:
//         std::vector<Renderable> m_renderables;
//         DirectionalLight m_directional_light;
//         std::vector<PointLight> m_point_lights;
//         ShadowBox m_shadow_box;
//         std::vector<Text> m_texts;
//         std::vector<Quad> m_quads;
//         std::vector<std::shared_ptr<PostProcessingStep>> m_post_processing_steps;
//         Skybox m_skybox;
//         Camera m_camera;
//         glm::vec3 m_camera_position {};
//         Camera2D m_camera_2d;

// #ifndef SM_BUILD_DISTRIBUTION
//         struct {
//             std::vector<DebugLine> debug_lines;
//             std::vector<Renderable*> renderables;
//             DirectionalLight* directional_light {};
//             std::vector<PointLight*> point_lights;
//             ShadowBox* shadow_box {};
//             std::vector<Text*> texts;
//             std::vector<Quad*> quads;
//         } m_debug;
// #endif

//         friend class Renderer;
//         friend class DebugRenderer;
//         friend class DebugUi;
//     };

    enum NodeFlag {
        Inherited,
        Enabled,
        Disabled
    };

    struct Transform3D {
        glm::vec3 position {};
        glm::vec3 rotation {};
        float scale {1.0f};
    };

    struct Transform2D {
        glm::vec2 position {};
        glm::vec2 scale {};
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

    class SceneNode3D {
    public:
        virtual ~SceneNode3D() = default;

        virtual SceneNode3DType type() const = 0;

        void add_node(std::shared_ptr<SceneNode3D> node);

        template<typename T>
        void traverse(T context, const std::function<void(const SceneNode3D*, T&)>& process) {
            process(this, context);

            for (const auto& child : m_children) {
                traverse(context, function);
            }
        }
    protected:
        struct TransformCastShadow : Transform3D {
            bool cast_shadow {};
        };

        void traverse_transform_cast_shadow(const std::function<void(const SceneNode3D*, TransformCastShadow&)>& process);

        std::vector<std::shared_ptr<SceneNode3D>> m_children;
        std::weak_ptr<SceneNode3D> m_parent;
    };

    class SceneNode2D {
    public:
        virtual ~SceneNode2D() = default;

        virtual SceneNode2DType type() const = 0;

        void add_node(std::shared_ptr<SceneNode2D> node);

        template<typename T>
        void traverse(T context, const std::function<void(const SceneNode2D*, T&)>& process) {
            process(this, context);

            for (const auto& child : m_children) {
                traverse(context, function);
            }
        }
    protected:
        std::vector<std::shared_ptr<SceneNode2D>> m_children;
        std::weak_ptr<SceneNode2D> m_parent;
    };

    struct DebugLine {
        glm::vec3 position1 {};
        glm::vec3 position2 {};
        glm::vec3 color {};
    };

    class Root3DNode : public SceneNode3D {
    public:
        void set_camera(const Camera& camera, glm::vec3 position);
        void set_skybox(const Skybox& skybox);
        void set_directional_light(const DirectionalLight& directional_light);
        void set_shadow_box(const ShadowBox& shadow_box);

        // Immediate mode debug API
        void debug_add_line(glm::vec3 position1, glm::vec3 position2, glm::vec3 color);
        void debug_add_lines(const std::vector<glm::vec3>& positions, glm::vec3 color);
        void debug_add_point(glm::vec3 position, glm::vec3 color);
        void debug_add_lamp(glm::vec3 position, glm::vec3 color);
        void debug_clear();
    private:
        void update_shadow_box();

        Camera m_camera;
        glm::vec3 m_camera_position {};
        Skybox m_skybox;
        DirectionalLight m_directional_light;
        ShadowBox m_shadow_box;

#ifndef SM_BUILD_DISTRIBUTION
        std::vector<DebugLine> m_debug_lines;
#endif

        friend class internal::Renderer;
        friend class internal::DebugRenderer;
        friend class internal::DebugUi;
    };

    class ModelNode : public SceneNode3D {
    public:
        ModelNode(std::shared_ptr<Mesh> mesh, std::shared_ptr<GlVertexArray> vertex_array, std::shared_ptr<MaterialInstance> material)
            : m_mesh(mesh), m_vertex_array(vertex_array), m_material(material) {}

        const utils::AABB& get_aabb() const { return m_mesh->get_aabb(); }
        std::shared_ptr<MaterialInstance> get_material() const { return m_material; }

        Transform3D transform;

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

    struct PointLightNode : public SceneNode3D, public PointLight {};  // Boo

    class Root2DNode : public SceneNode2D {
    public:
        void set_camera(const Camera2D& camera);
    private:
        Camera2D m_camera;
    };

    class ImageNode : public SceneNode2D {
    public:
        explicit ImageNode(std::shared_ptr<GlTexture> texture)
            : m_texture(texture) {}

        Transform2D transform;
    private:
        std::shared_ptr<GlTexture> m_texture;
    };

    class TextNode : public SceneNode2D {
    public:
        explicit TextNode(std::shared_ptr<Font> font)
            : m_font(font) {}

        Transform2D transform;  // FIXME uniform scale
        glm::vec3 color {};
        std::string text;
    private:
        std::shared_ptr<Font> m_font;
    };

    struct Scene {
        std::shared_ptr<Root3DNode> root_3d_node;
        std::shared_ptr<Root2DNode> root_2d_node;
    };
}
