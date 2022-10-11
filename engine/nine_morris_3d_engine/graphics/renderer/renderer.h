#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/application/events.h"
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
#include "nine_morris_3d_engine/other/encrypt.h"

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
    struct Model {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        float scale = 1.0f;

        std::shared_ptr<VertexArray> vertex_array;
        std::shared_ptr<IndexBuffer> index_buffer;
        std::shared_ptr<MaterialInstance> material;

        std::optional<glm::vec3> outline_color;
        std::optional<hover::Id> id;
        bool cast_shadow = false;
    };

    struct Quad {
        glm::vec3 position = glm::vec3(0.0f);
        float scale = 1.0f;

        std::shared_ptr<Texture> texture;
    };

    Renderer(Application* app);
    ~Renderer();

    void render();

    void add_model(std::shared_ptr<Model> model);
    void remove_model(std::shared_ptr<Model> model);

    void add_quad(std::shared_ptr<Quad> quad);
    void remove_quad(std::shared_ptr<Quad> quad);

    void clear();

    void setup_shader(std::shared_ptr<Shader> shader);
    void add_post_processing(std::shared_ptr<PostProcessingStep> post_processing_step);

    void set_scene_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void set_shadow_map_framebuffer(int size);
    void set_skybox(std::shared_ptr<Texture3D> texture);
    void set_camera(Camera* camera);

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
    void draw_model(const Model* model);
    void draw_model_with_outline(const Model* model);
    void draw_models();
    void draw_models_with_outline();
    void draw_models_to_depth_buffer();
    void draw_quads();
    void setup_shadows();
    void setup_uniform_buffers();
    void check_hovered_id(int x, int y);
    void cache_camera();
    void on_window_resized(const WindowResizedEvent& event);

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
        std::shared_ptr<Buffer> skybox_buffer;
        std::shared_ptr<VertexArray> screen_quad_vertex_array;
        std::shared_ptr<Buffer> screen_quad_buffer;
        std::shared_ptr<VertexArray> quad_vertex_array;
        std::shared_ptr<Buffer> quad_buffer;
#ifdef PLATFORM_GAME_DEBUG
        std::shared_ptr<VertexArray> origin_vertex_array;
        std::shared_ptr<Buffer> origin_buffer;
#endif

        std::shared_ptr<Texture3D> skybox_texture;

        std::shared_ptr<Framebuffer> scene_framebuffer;
        std::shared_ptr<Framebuffer> depth_map_framebuffer;
        std::shared_ptr<Framebuffer> intermediate_framebuffer;

        std::array<std::shared_ptr<PixelBuffer>, 4> pixel_buffers;
    } storage;

    struct CameraCache {
        glm::mat4 projection_matrix = glm::mat4(1.0f);
        glm::mat4 view_matrix = glm::mat4(1.0f);
        glm::mat4 projection_view_matrix = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0.0f);
    } camera_cache;

    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Quad>> quads;

    hover::Id hovered_id = hover::null;
    FramebufferReader<4> reader;
    Camera* camera = nullptr;  // Don't use this directly

    int shadow_map_size = 4096;

    PostProcessingContext post_processing_context;

    const char* SHADOW_VERTEX_SHADER = ENCR("engine_data/shaders/shadow.vert");
    const char* SHADOW_FRAGMENT_SHADER = ENCR("engine_data/shaders/shadow.frag");
    const char* SCREEN_QUAD_VERTEX_SHADER = ENCR("engine_data/shaders/screen_quad.vert");
    const char* SCREEN_QUAD_FRAGMENT_SHADER = ENCR("engine_data/shaders/screen_quad.frag");
    const char* OUTLINE_VERTEX_SHADER = ENCR("engine_data/shaders/outline.vert");
    const char* OUTLINE_FRAGMENT_SHADER = ENCR("engine_data/shaders/outline.frag");
    const char* SKYBOX_VERTEX_SHADER = ENCR("engine_data/shaders/skybox.vert");
    const char* SKYBOX_FRAGMENT_SHADER = ENCR("engine_data/shaders/skybox.frag");
    const char* QUAD3D_VERTEX_SHADER = ENCR("engine_data/shaders/quad3d.vert");
    const char* QUAD3D_FRAGMENT_SHADER = ENCR("engine_data/shaders/quad3d.frag");
#ifdef PLATFORM_GAME_DEBUG
    const char* ORIGIN_VERTEX_SHADER = "engine_data/shaders/origin.vert";
    const char* ORIGIN_FRAGMENT_SHADER = "engine_data/shaders/origin.frag";
#endif

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
