#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <initializer_list>
#include <memory>
#include <utility>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/file_system.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/post_processing_context.hpp"
#include "nine_morris_3d_engine/graphics/renderable.hpp"
#include "nine_morris_3d_engine/graphics/light.hpp"
#include "nine_morris_3d_engine/graphics/camera.hpp"
#include "nine_morris_3d_engine/graphics/shader_library.hpp"
#include "nine_morris_3d_engine/graphics/scene.hpp"

namespace sm {
    class Application;
    class Ctx;
    class Renderer;
    class FileSystem;
    class GlVertexArray;
    class GlVertexBuffer;
    class GlUniformBuffer;
    class Font;

    class DebugRenderer {
    private:
        DebugRenderer() = default;
        DebugRenderer(const FileSystem& fs, Renderer& renderer);

        void render(const Scene& scene);

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

        friend class Renderer;
    };

    class Renderer {
    private:
        Renderer(int width, int height, int samples, const FileSystem& fs, const ShaderLibrary& shd);
    public:
        // Retained
        void register_shader(std::shared_ptr<GlShader> shader);  // TODO maybe make these automatic
        void register_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer);

        void render(const Scene& scene, int width, int height);
    private:
        void pre_setup();
        void post_setup();
        void post_processing(const Scene& scene);
        void end_3d_rendering(const Scene& scene, int width, int height);
        void screen_quad(unsigned int texture);
        void resize_framebuffers(int width, int height);

        // Draw functions
        void draw_renderables(const Scene& scene);
        void draw_renderable(const Renderable& renderable);

        void draw_renderables_outlined(const Scene& scene);
        void draw_renderable_outlined(const Renderable& renderable);

        void draw_renderables_to_shadow_map(const Scene& scene);
        void draw_skybox(const Scene& scene);

        struct TextBatch {
            std::weak_ptr<Font> wfont;
            std::vector<Text> texts;
        };

        void draw_texts(const Scene& scene);
        void draw_text_batch(const Scene& scene, const TextBatch& batch);

        void draw_quads(const Scene& scene);
        void draw_quad(glm::vec2 position, glm::vec2 size, glm::vec2 scale, unsigned int texture);
        void begin_quads_batch();
        void end_quads_batch();
        void flush_quads_batch();

        // Helper functions
        void setup_point_light_uniform_buffer(const Scene& scene, std::shared_ptr<GlUniformBuffer> uniform_buffer);
        void setup_light_space_uniform_buffer(const Scene& scene, std::shared_ptr<GlUniformBuffer> uniform_buffer);
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
                std::vector<Font::CharacterBuffer> batch_buffer;
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
