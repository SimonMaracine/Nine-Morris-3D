#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <utility>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/internal/file_system.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/graphics/internal/shader_library.hpp"
#include "nine_morris_3d_engine/graphics/internal/post_processing_context.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"
#include "nine_morris_3d_engine/graphics/opengl/framebuffer.hpp"
#include "nine_morris_3d_engine/graphics/scene.hpp"
#include "nine_morris_3d_engine/graphics/font.hpp"

namespace sm {
    class GlVertexArray;
    class GlVertexBuffer;
    class GlUniformBuffer;
}

namespace sm::internal {
    // Used by the application to specify a few renderer parameters
    struct RendererSpecification {
        int samples {1};  // Anti-aliasing
        int scale {1};  // Global scale
        int shadow_map_size {2048};
    };

    // Main class responsible for rendering stuff on the screen
    class Renderer {
    public:
        Renderer(int width, int height, const FileSystem& fs, const ShaderLibrary& shd);

        // Retrieve the default font; it is null until the renderer is fully initialized
        std::shared_ptr<Font> get_default_font() const;

        // Color correction (sRGB)
        void set_color_correction(bool enable);
        bool get_color_correction() const;

        // Set background color
        void set_clear_color(glm::vec3 color);

        // Set anti-aliasing
        void set_samples(int width, int height, int samples);

        // Set global scale
        void set_scale(const FileSystem& fs, int scale);

        // Set the size of the shadow map; must be a power of 2
        void set_shadow_map_size(int size);

        // Fully initialize the renderer
        void initialize(int width, int height, const FileSystem& fs, const RendererSpecification& specification = {});

        // Register refereneces to shaders and framebuffers in order for the renderer to do special things
        void register_shader(std::shared_ptr<GlShader> shader);
        void register_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer);

        // Do the actual rendering
        // Invokes the necessary OpenGL rendering functions
        void render(const Scene& scene, int width, int height);

        // Pre and post initialization for the renderer
        // Must be called between scene changes or similar siuations
        void pre_setup();
        void post_setup();

        // Resize all resizable registered framebuffers
        // Called when the window size changes
        void resize_framebuffers(int width, int height);

        // Get the maximum supported point lights
        static std::size_t get_max_point_lights();
    private:
        void set_and_upload_uniform_buffer_data(const Scene& scene);
        void post_processing(const Scene& scene);
        void finish_3d(const Scene& scene, int width, int height);
        void screen_quad(const GlShader* shader, unsigned int texture);
        void setup_shader_uniform_buffers(std::shared_ptr<GlShader> shader);
        void clear_expired_resources();

        // Draw functions
        void draw_models(const Scene& scene);
        void draw_model(const ModelNode* model_node, const Context3D& context);

        void draw_models_outlined(const Scene& scene);
        void draw_model_outlined(const Scene& scene, const OutlinedModelNode* outlined_model_node, const Context3D& context);

        void draw_models_to_shadow_map(const Scene& scene);
        void draw_skybox(const Scene& scene);

        struct TextBatch {
            std::shared_ptr<Font> font;
            std::vector<std::pair<const TextNode*, Context2D>> texts;
        };

        void draw_texts(const Scene& scene);
        void draw_text_batch(const Scene& scene, const TextBatch& batch);

        void draw_images(const Scene& scene);
        void draw_image(const ImageNode* image_node, const Context2D& context);
        void begin_images_batch();
        void end_images_batch();
        void flush_images_batch();

        // Helper functions
        void setup_point_light_uniform_buffer(const Scene& scene, std::shared_ptr<GlUniformBuffer> uniform_buffer);
        void setup_light_space_uniform_buffer(const Scene& scene, std::shared_ptr<GlUniformBuffer> uniform_buffer);
        void setup_scene_framebuffer(int width, int height, int samples);
        void setup_shadow_framebuffer(int size);
        void setup_default_font(const FileSystem& fs, int scale);
        std::shared_ptr<GlIndexBuffer> initialize_quads_index_buffer();

        struct QuadVertex {
            glm::vec2 position {};
            glm::vec2 texture_coordinate {};
            int texture_index {};
        };

        struct {
            std::shared_ptr<GlFramebuffer> scene_framebuffer;
            std::shared_ptr<GlFramebuffer> final_framebuffer;
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

            struct {
                std::vector<TextBatch> batches;
                std::vector<Font::CharacterBuffer> batch_buffer;
                std::vector<glm::mat4> batch_matrices;
                std::vector<glm::vec3> batch_colors;
            } text;

            struct {
                std::unique_ptr<QuadVertex[]> buffer;
                QuadVertex* buffer_pointer {};
                std::array<unsigned int, 8> textures {};
                std::size_t texture_index {};
                std::size_t quad_count {};
            } quad;

            std::vector<std::weak_ptr<GlShader>> shaders;
            std::vector<std::weak_ptr<GlFramebuffer>> framebuffers;
            std::unordered_map<unsigned int, std::weak_ptr<GlUniformBuffer>> uniform_buffers;
        } m_storage;

        PostProcessingContext m_post_processing_context;
        glm::vec3 m_clear_color {};
        bool m_color_correction {true};

#ifndef SM_BUILD_DISTRIBUTION
        void debug_initialize(const FileSystem& fs);
        void debug_render(const Scene& scene);

        struct BufferVertex {
            glm::vec3 position;
            glm::vec3 color;
        };

        struct {
            std::shared_ptr<GlShader> shader;
            std::weak_ptr<GlVertexBuffer> wvertex_buffer;
            std::unique_ptr<GlVertexArray> vertex_array;

            std::vector<BufferVertex> lines_buffer;
        } m_debug_storage;
#endif

        static constexpr unsigned int PROJECTION_VIEW_UNIFORM_BLOCK_BINDING {0};
        static constexpr unsigned int DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING {1};
        static constexpr unsigned int VIEW_UNIFORM_BLOCK_BINDING {2};
        static constexpr unsigned int POINT_LIGHT_UNIFORM_BLOCK_BINDING {3};
        static constexpr unsigned int LIGHT_SPACE_UNIFORM_BLOCK_BINDING {4};
        static constexpr std::size_t SHADER_MAX_POINT_LIGHTS {4};
        static constexpr std::size_t SHADER_MAX_BATCH_TEXTS {32};  // This should never reach the limit
        static constexpr int SHADOW_MAP_UNIT {2};
        static constexpr std::size_t MAX_QUAD_COUNT {1000};
        static constexpr std::size_t MAX_QUADS_BUFFER_SIZE {MAX_QUAD_COUNT * 4 * sizeof(QuadVertex)};
        static constexpr std::size_t MAX_QUADS_INDICES {MAX_QUAD_COUNT * 6};
    };
}
