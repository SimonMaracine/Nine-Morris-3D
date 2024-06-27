#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <initializer_list>
#include <memory>
#include <utility>

#include <glm/glm.hpp>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/file_system.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/framebuffer.hpp"
#include "engine/graphics/opengl/texture.hpp"
#include "engine/graphics/post_processing.hpp"
#include "engine/graphics/renderable.hpp"
#include "engine/graphics/light.hpp"
#include "engine/graphics/camera.hpp"
#include "engine/graphics/shader_library.hpp"

namespace sm {
    class Application;
    class Ctx;
    class Renderer;
    class FileSystem;
    class GlVertexArray;
    class GlVertexBuffer;
    class GlUniformBuffer;
    class Font;

    struct DebugRenderer {
    public:
        DebugRenderer() = default;
        DebugRenderer(const FileSystem& fs, Renderer& renderer);
    private:
        void render();
        void clear();

        struct BufferVertex {
            glm::vec3 position;
            glm::vec3 color;
        };

        struct {
            std::shared_ptr<GlShader> shader;
            std::weak_ptr<GlVertexBuffer> wvertex_buffer;
            std::unique_ptr<GlVertexArray> vertex_array;

            std::vector<BufferVertex> lines_buffer;
        } storage;

        struct Line {
            glm::vec3 position1;
            glm::vec3 position2;
            glm::vec3 color;
        };

        struct {
            std::vector<Line> lines;
        } scene;

        friend class Renderer;
    };

    class Renderer {
    private:
        Renderer(int width, int height, int samples, const FileSystem& fs, const ShaderLibrary& shd);
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

        template<typename T, typename... Args>
        void add_post_processing(Args&&... args) {
            post_processing_context.steps.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        // 3D API
        void add_renderable(const Renderable& renderable);
        void add_light(const DirectionalLight& light);
        void add_light(const PointLight& light);

        // 2D API
        void add_text(const Text& text);
        void add_info_text(float fps);
        void add_quad(const Quad& quad);

        // Debug API
        void debug_add_line(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& color);
        void debug_add_lines(const std::vector<glm::vec3>& points, const glm::vec3& color);
        void debug_add_lines(std::initializer_list<glm::vec3> points, const glm::vec3& color);
        void debug_add_point(const glm::vec3& position, const glm::vec3& color);
        void debug_add_lamp(const glm::vec3& position, const glm::vec3& color);
    private:
        void pre_setup();
        void post_setup();
        void render(int width, int height);
        void post_processing();
        void end_3d_rendering(int width, int height);
        void screen_quad(unsigned int texture);
        void clear();
        void resize_framebuffers(int width, int height);

        // Draw functions
        void draw_renderables();
        void draw_renderable(const Renderable& renderable);

        void draw_renderables_outlined();
        void draw_renderable_outlined(const Renderable& renderable);

        void draw_renderables_to_depth_buffer();
        void draw_skybox();

        struct TextBatch {
            std::weak_ptr<Font> wfont;
            std::vector<Text> texts;
        };

        void draw_texts();
        void draw_text_batch(const TextBatch& batch);

        void draw_quads();
        void draw_quad(glm::vec2 position, glm::vec2 size, glm::vec2 scale, unsigned int texture);
        void begin_quads_batch();
        void end_quads_batch();
        void flush_quads_batch();

        // Helper functions
        void setup_point_light_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer);
        void setup_light_space_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer);
        std::shared_ptr<GlIndexBuffer> initialize_quads_index_buffer();
        static glm::mat4 get_renderable_transform(const Renderable& renderable);

        struct QuadVertex {
            glm::vec2 position {};
            glm::vec2 texture_coordinate {};
            int texture_index {};
        };

        struct {
            std::shared_ptr<GlFramebuffer> scene_framebuffer;
            std::shared_ptr<GlFramebuffer> intermediate_framebuffer;
            std::shared_ptr<GlFramebuffer> shadow_map_framebuffer;

            std::unique_ptr<GlShader> screen_quad_shader;
            std::shared_ptr<GlShader> shadow_shader;
            std::unique_ptr<GlShader> text_shader;
            std::unique_ptr<GlShader> quad_shader;
            std::unique_ptr<GlShader> skybox_shader;
            std::shared_ptr<GlShader> outline_shader;

            std::unique_ptr<GlVertexArray> screen_quad_vertex_array;
            std::unique_ptr<GlVertexArray> text_vertex_array;
            std::unique_ptr<GlVertexArray> quad_vertex_array;
            std::unique_ptr<GlVertexArray> skybox_vertex_array;

            std::shared_ptr<Font> default_font;

            std::shared_ptr<GlTextureCubemap> skybox_texture;

            std::weak_ptr<GlVertexBuffer> wtext_vertex_buffer;
            std::weak_ptr<GlVertexBuffer> wquad_vertex_buffer;

            std::unordered_map<unsigned int, std::weak_ptr<GlUniformBuffer>> uniform_buffers;
            std::weak_ptr<GlUniformBuffer> wprojection_view_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wdirectional_light_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wview_position_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wpoint_light_uniform_buffer;
            std::weak_ptr<GlUniformBuffer> wlight_space_uniform_buffer;

            struct {
                std::vector<TextBatch> batches;
                std::vector<unsigned char> batch_buffer;
                std::vector<glm::mat4> batch_matrices;
                std::vector<glm::vec3> batch_colors;
            } text;

            struct {
                std::unique_ptr<QuadVertex[]> buffer;
                QuadVertex* buffer_pointer {nullptr};

                std::array<unsigned int, 8> texture_slots {};
                std::size_t texture_slot_index {};

                std::size_t quad_count {};
            } quad;

            struct {
                std::vector<std::weak_ptr<GlShader>> shaders;
                std::vector<std::weak_ptr<GlFramebuffer>> framebuffers;
            } scene;
        } storage;

        PostProcessingContext post_processing_context;

        struct {
            std::vector<Renderable> renderables;
            DirectionalLight directional_light;
            std::vector<PointLight> point_lights;
            std::vector<Text> texts;
            std::vector<Quad> quads;

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
        } scene;

#ifndef SM_BUILD_DISTRIBUTION
        DebugRenderer debug;
#endif

        static constexpr unsigned int PROJECTON_VIEW_UNIFORM_BLOCK_BINDING {0};
        static constexpr unsigned int DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING {1};
        static constexpr unsigned int VIEW_POSITION_BLOCK_BINDING {2};
        static constexpr unsigned int POINT_LIGHT_BLOCK_BINDING {3};
        static constexpr unsigned int LIGHT_SPACE_BLOCK_BINDING {4};
        static constexpr std::size_t SHADER_MAX_POINT_LIGHTS {4};
        static constexpr std::size_t SHADER_MAX_BATCH_TEXTS {32};  // This should never reach the limit
        static constexpr int SHADOW_MAP_UNIT {1};
        static constexpr std::size_t MAX_QUAD_COUNT {1000};
        static constexpr std::size_t MAX_QUADS_BUFFER_SIZE {MAX_QUAD_COUNT * 4 * sizeof(QuadVertex)};
        static constexpr std::size_t MAX_QUADS_INDICES {MAX_QUAD_COUNT * 6};
        static constexpr std::size_t MAX_QUADS_TEXTURES {8};

        friend class Application;
        friend class Ctx;
    };
}
