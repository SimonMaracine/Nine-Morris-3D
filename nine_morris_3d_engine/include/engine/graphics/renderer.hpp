#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <initializer_list>

#include <glm/glm.hpp>

#include "engine/application_base/file_system.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/opengl/texture.hpp"
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
    class Font;

    class Renderer {
    private:
        Renderer(int width, int height, int samples, const FileSystem& fs);
    public:
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;

        void capture(const Camera& camera, const glm::vec3& position);
        void capture(const Camera2D& camera_2d);
        void skybox(std::shared_ptr<GlTextureCubemap> texture);
        void shadows(float left, float right, float bottom, float top, float lens_near, float lens_far, const glm::vec3& position);
        void register_shader(std::shared_ptr<GlShader> shader);
        void register_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer);

        // 3D API
        void add_renderable(const Renderable& renderable);
        void add_light(const DirectionalLight& light);
        void add_light(const PointLight& light);

        // 2D API
        void add_text(const Text& text);
        void add_info_text(float fps);

        // Debug API
        void debug_add_line(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& color);
        void debug_add_lines(const std::vector<glm::vec3>& points, const glm::vec3& color);
        void debug_add_lines(std::initializer_list<glm::vec3> points, const glm::vec3& color);
        void debug_add_point(const glm::vec3& position, const glm::vec3& color);
        void debug_add_lamp(const glm::vec3& position, const glm::vec3& color);
    private:
        void render();
        void pre_setup();
        void post_setup();
        void clear();
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

        void draw_renderables_to_depth_buffer();
        void draw_skybox();

        struct TextBatch {
            std::weak_ptr<Font> font;
            std::vector<Text> objects;
        };

        void draw_texts();
        void draw_text_batch(const TextBatch& batch);

        // Helper functions
        void setup_point_light_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer);
        void setup_light_space_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer);
        static glm::mat4 get_renderable_transform(const Renderable& renderable);

        struct {
            std::shared_ptr<GlFramebuffer> scene_framebuffer;
            std::shared_ptr<GlFramebuffer> intermediate_framebuffer;
            std::shared_ptr<GlFramebuffer> shadow_map_framebuffer;

            std::unique_ptr<GlShader> screen_quad_shader;
            std::shared_ptr<GlShader> shadow_shader;
            std::unique_ptr<GlShader> text_shader;
            std::unique_ptr<GlShader> skybox_shader;

            std::unique_ptr<GlVertexArray> screen_quad_vertex_array;
            std::unique_ptr<GlVertexArray> skybox_vertex_array;
            std::unique_ptr<GlVertexArray> text_vertex_array;

            std::shared_ptr<Font> default_font;

            std::shared_ptr<GlTextureCubemap> skybox_texture;

            std::weak_ptr<GlVertexBuffer> wtext_vertex_buffer;

            std::unordered_map<unsigned int, std::weak_ptr<GlUniformBuffer>> uniform_buffers;
            std::weak_ptr<GlUniformBuffer> wprojection_view_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wdirectional_light_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wview_position_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wpoint_light_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wlight_space_uniform_buffer;

            std::vector<TextBatch> text_batches;
            std::vector<unsigned char> texts_buffer;
        } storage;

        PostProcessingContext post_processing_context;  // TODO implement

        struct {
            std::vector<Renderable> renderables;
            DirectionalLight directional_light;
            std::vector<PointLight> point_lights;
            std::vector<Text> texts;

            struct {
                glm::mat4 view_matrix {1.0f};
                glm::mat4 projection_matrix {1.0f};
                glm::mat4 projection_view_matrix {1.0f};
                glm::vec3 position {};
            } camera;

            struct {
                glm::mat4 projection_matrix {1.0f};
            } camera_2d;

            struct {
                float left {};
                float right {};
                float bottom {};
                float top {};
                float lens_near {1.0f};
                float lens_far {1.0f};
                glm::vec3 position {};
            } light_space;
        } scene_list;

        struct {
            std::vector<std::weak_ptr<GlShader>> shaders;
            std::vector<std::weak_ptr<GlFramebuffer>> framebuffers;
        } scene_data;

        // Debug ---------------------------------------------------------------

        void debug_initialize(const FileSystem& fs);
        void debug_render();
        void debug_clear();

        struct BufferVertex {
            glm::vec3 position;
            glm::vec3 color;
        };

        struct {
            std::shared_ptr<GlShader> shader;
            std::weak_ptr<GlVertexBuffer> wvertex_buffer;
            std::unique_ptr<GlVertexArray> vertex_array;

            std::vector<BufferVertex> lines_buffer;
        } debug_storage;

        struct Line {
            glm::vec3 position1;
            glm::vec3 position2;
            glm::vec3 color;
        };

        struct {
            std::vector<Line> lines;
        } debug_scene_list;

        friend class Application;
        friend class Ctx;
    };
}
