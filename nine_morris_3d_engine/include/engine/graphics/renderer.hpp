#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "engine/application_base/file_system.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/post_processing.hpp"
#include "engine/graphics/renderable.hpp"
#include "engine/graphics/light.hpp"
#include "engine/graphics/camera.hpp"

namespace sm {
    class Application;
    class Ctx;
    class FileSystem;
    class GlVertexArray;
    class GlVertexBuffer;
    class GlUniformBuffer;

    class Renderer {
    private:
        Renderer(int width, int height, const FileSystem& fs);
    public:
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;

        void capture(const Camera& camera, const glm::vec3& position);
        void add_shader(std::shared_ptr<GlShader> shader);
        void add_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer);

        // 3D API
        void add_renderable(const Renderable& renderable);
        void add_light(const DirectionalLight& light);
        void add_light(const PointLight& light);

        // Debug API
        void debug_add_line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color);
        void debug_add_lines(const std::vector<glm::vec3>& points, const glm::vec3& color);

        void debug_add_point(const glm::vec3& p, const glm::vec3& color);
        void debug_add_lamp(const glm::vec3& position, const glm::vec3& color);
    private:
        void render(int width, int height);
        void prerender_setup();
        void postrender_setup();

        void resize_framebuffers(int width, int height);

        // Render functions
        void screen_quad(unsigned int texture);
        void post_processing();
        void end_rendering();

        // Draw functions
        void draw_renderables();
        void draw_renderable(const Renderable& renderable);

        void draw_renderables_outlined();
        void draw_renderable_outlined(const Renderable& renderable);

        // Helper functions
        void setup_point_light_uniform_buffer(const std::shared_ptr<GlUniformBuffer> uniform_buffer);  // FIXME

        struct {
            std::shared_ptr<GlFramebuffer> scene_framebuffer;

            std::unique_ptr<GlShader> screen_quad_shader;
            std::unique_ptr<GlVertexArray> screen_quad_vertex_array;

            std::unordered_map<unsigned int, std::weak_ptr<GlUniformBuffer>> uniform_buffers;
            std::weak_ptr<GlUniformBuffer> projection_view_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> directional_light_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> view_position_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> point_light_uniform_buffer;
        } storage;

        PostProcessingContext post_processing_context;  // TODO implement

        struct {
            glm::mat4 view_matrix {1.0f};
            glm::mat4 projection_matrix {1.0f};
            glm::mat4 projection_view_matrix {1.0f};
            glm::vec3 position {};
        } camera;

        struct SceneList {
            std::vector<Renderable> renderables;
            DirectionalLight directional_light;
            std::vector<PointLight> point_lights;

            void clear();
        } scene_list;

        struct {
            std::vector<std::weak_ptr<GlShader>> shaders;
            std::vector<std::weak_ptr<GlFramebuffer>> framebuffers;
        } scene_data;

        // Debug stuff
        void debug_initialize(const FileSystem& fs);
        void debug_render();
        void debug_clear();

        struct {
            std::shared_ptr<GlShader> shader;

            std::weak_ptr<GlVertexBuffer> vertex_buffer;
            std::unique_ptr<GlVertexArray> vertex_array;
        } debug_storage;

        struct BufferVertexStruct {
            glm::vec3 position;
            glm::vec3 color;
        };

        struct Line {
            glm::vec3 p1;
            glm::vec3 p2;
            glm::vec3 color;
        };

        std::vector<Line> debug_scene_list;

        friend class Application;
        friend class Ctx;
    };
}
