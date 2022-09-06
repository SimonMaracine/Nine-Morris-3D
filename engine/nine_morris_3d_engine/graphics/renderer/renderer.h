#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/ecs/internal_components.h"
#include "nine_morris_3d_engine/graphics/renderer/framebuffer_reader.h"
#include "nine_morris_3d_engine/graphics/renderer/font.h"
#include "nine_morris_3d_engine/graphics/renderer/camera.h"
#include "nine_morris_3d_engine/graphics/renderer/material.h"
#include "nine_morris_3d_engine/graphics/renderer/light.h"
#include "nine_morris_3d_engine/graphics/renderer/hover.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/texture.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"

class Application;
class PostProcessingStep;

struct PostProcessingContext {
    std::vector<std::shared_ptr<PostProcessingStep>> steps;
    GLuint last_texture = 0;  // Last texture at any moment in the processing pipeline
    std::vector<GLuint> textures;  // All textures in order
    GLuint original_texture = 0;
};

class Renderer {
public:
    Renderer(Application* app);
    ~Renderer();

    void render();
    void on_window_resized(const WindowResizedEvent& event);

    void setup_shader(std::shared_ptr<Shader> shader);
    void add_post_processing(std::shared_ptr<PostProcessingStep> post_processing_step);

    void clear();

    void set_scene_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void set_skybox(std::shared_ptr<Texture3D> texture);
    void set_depth_map_framebuffer(int size);

    // Framebuffer management functions
    void add_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void purge_framebuffers();

    hover::Id get_hovered_id() { return hovered_id; }
    PostProcessingContext& get_post_processing_context() { return post_processing_context; }
    UniformBlockSpecification& get_projection_view_uniform_block() { return storage.projection_view_uniform_block; }
    UniformBlockSpecification& get_light_uniform_block() { return storage.light_uniform_block; }
    UniformBlockSpecification& get_light_view_position_uniform_block() { return storage.light_view_position_uniform_block; }
    UniformBlockSpecification& get_light_space_uniform_block() { return storage.light_space_uniform_block; }
    std::shared_ptr<Framebuffer> get_scene_framebuffer() { return storage.scene_framebuffer; }

#ifdef PLATFORM_GAME_DEBUG
    std::shared_ptr<Shader> get_origin_shader() { return storage.origin_shader; }
#endif
    std::shared_ptr<Shader> get_outline_shader() { return storage.outline_shader; }
    std::shared_ptr<Shader> get_quad3d_shader() { return storage.quad3d_shader; }
    std::shared_ptr<Shader> get_screen_quad_shader() { return storage.screen_quad_shader; }
    std::shared_ptr<Shader> get_shadow_shader() { return storage.shadow_shader; }
    std::shared_ptr<Shader> get_skybox_shader() { return storage.skybox_shader; }

    bool origin = false;  // This does nothing in release mode
    DirectionalLight light;

    struct LightSpace {
        float left = 0.0f;
        float right = 0.0f;
        float bottom = 0.0f;
        float top = 0.0f;
        float near = 1.0f;
        float far = 1.0f;
        float light_divisor = 1.0f;
    } light_space;
private:
    void draw_screen_quad(GLuint texture);
    void post_processing();
    void end_rendering();
    void draw_origin();
    void draw_skybox();
    void draw_model(const ModelComponent& model_c, const TransformComponent& transform_c);
    void draw_model_with_outline(const ModelComponent& model_c, const TransformComponent& transform_c,
            const OutlineComponent& outline_c);
    void draw_models_to_depth_buffer();
    void draw_models_normal();
    void draw_models_with_outline();
    void draw_quads();
    void setup_shadows();
    void setup_uniform_buffers();
    void check_hovered_id(int x, int y);
    void cache_camera();

    struct {
        std::shared_ptr<UniformBuffer> projection_view_uniform_buffer;
        std::shared_ptr<UniformBuffer> light_uniform_buffer;
        std::shared_ptr<UniformBuffer> light_view_position_uniform_buffer;
        std::shared_ptr<UniformBuffer> light_space_uniform_buffer;

        UniformBlockSpecification projection_view_uniform_block;
        UniformBlockSpecification light_uniform_block;
        UniformBlockSpecification light_view_position_uniform_block;
        UniformBlockSpecification light_space_uniform_block;

        std::shared_ptr<Shader> skybox_shader;
        std::shared_ptr<Shader> screen_quad_shader;
        std::shared_ptr<Shader> quad3d_shader;
        std::shared_ptr<Shader> shadow_shader;
        std::shared_ptr<Shader> outline_shader;
#ifdef PLATFORM_GAME_DEBUG
        std::shared_ptr<Shader> origin_shader;
#endif

        std::shared_ptr<VertexArray> skybox_vertex_array;
        std::shared_ptr<VertexArray> quad_vertex_array;
#ifdef PLATFORM_GAME_DEBUG
        std::shared_ptr<VertexArray> origin_vertex_array;
#endif

        std::shared_ptr<Texture3D> skybox_texture;

        std::shared_ptr<Framebuffer> scene_framebuffer;
        std::shared_ptr<Framebuffer> depth_map_framebuffer;
        std::shared_ptr<Framebuffer> intermediate_framebuffer;

        std::array<std::shared_ptr<PixelBuffer>, 4> pixel_buffers;
    } storage;

    struct {
        glm::mat4 projection_matrix = glm::mat4(1.0f);
        glm::mat4 view_matrix = glm::mat4(1.0f);
        glm::mat4 projection_view_matrix = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0.0f);
    } camera_cache;

    hover::Id hovered_id = hover::null;
    FramebufferReader<4> reader;

    int shadow_map_size = 4096;

    PostProcessingContext post_processing_context;

    // Keep track of all framebuffers to resize them, if needed
    std::vector<std::weak_ptr<Framebuffer>> framebuffers;

    std::string SHADOW_VERTEX_SHADER = "data/shaders/internal/shadow.vert.dat";
    std::string SHADOW_FRAGMENT_SHADER = "data/shaders/internal/shadow.frag.dat";
    std::string SCREEN_QUAD_VERTEX_SHADER = "data/shaders/internal/screen_quad.vert.dat";
    std::string SCREEN_QUAD_FRAGMENT_SHADER = "data/shaders/internal/screen_quad.frag.dat";
    std::string OUTLINE_VERTEX_SHADER = "data/shaders/internal/outline.vert.dat";
    std::string OUTLINE_FRAGMENT_SHADER = "data/shaders/internal/outline.frag.dat";
    std::string SKYBOX_VERTEX_SHADER = "data/shaders/internal/skybox.vert.dat";
    std::string SKYBOX_FRAGMENT_SHADER = "data/shaders/internal/skybox.frag.dat";
    std::string QUAD3D_VERTEX_SHADER = "data/shaders/internal/quad3d.vert.dat";
    std::string QUAD3D_FRAGMENT_SHADER = "data/shaders/internal/quad3d.frag.dat";
#ifdef PLATFORM_GAME_DEBUG
    std::string ORIGIN_VERTEX_SHADER = "data/shaders/internal/origin.vert";
    std::string ORIGIN_FRAGMENT_SHADER = "data/shaders/internal/origin.frag";
#endif

    // Reference to application
    Application* app = nullptr;

    friend class Application;
};

namespace render_helpers {
    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    void clear(int buffers);
    void viewport(int width, int height);
    void clear_color(float red, float green, float blue);
    void bind_texture_2d(GLuint texture, int unit);
    void draw_arrays(int count);
}
