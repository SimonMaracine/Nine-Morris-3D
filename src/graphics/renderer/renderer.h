#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "application/platform.h"
#include "application/events.h"
#include "graphics/renderer/framebuffer_reader.h"
#include "graphics/renderer/font.h"
#include "graphics/renderer/camera.h"
#include "graphics/renderer/material.h"
#include "graphics/renderer/light.h"
#include "graphics/renderer/hoverable.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/buffer.h"
#include "graphics/renderer/opengl/shader.h"
#include "graphics/renderer/opengl/texture.h"
#include "graphics/renderer/opengl/framebuffer.h"

class Application;

class Renderer {
public:
    struct Model {
        std::shared_ptr<VertexArray> vertex_array;
        int index_count = 0;
        std::shared_ptr<MaterialInstance> material;

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        float scale = 1.0f;

        // To be used with outline rendering
        glm::vec3 outline_color = glm::vec3(0.0f);

        unsigned int handle = 0;
    };

    struct Quad {
        std::shared_ptr<Texture> texture;
    
        glm::vec3 position = glm::vec3(0.0f);
        float scale = 1.0f;

        unsigned int handle = 0;
    };

    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    enum {
        WithOutline = 1 << 0,
        CastShadow = 1 << 1,
    };

    struct PostProcessing {
        union {
            struct {
                std::shared_ptr<Framebuffer> framebuffer;
                std::shared_ptr<Shader> shader;
            } normal;

            struct {
                GLuint framebuffer;
            } end;
        };

        bool enabled = true;
    };

    Renderer(Application* app);
    ~Renderer();

    void render();
    void on_window_resized(events::WindowResizedEvent& event);

    void add_model(Model& model, int options = 0);
    void remove_model(unsigned int handle);
    void update_model(Model& model, int options = 0);

    void add_quad(Quad& quad);
    void remove_quad(unsigned int handle);

    void setup_shader(std::shared_ptr<Shader> shader);
    void add_post_processing(const PostProcessing& post_processing);
    void end_post_processing_list();

    void clear();

    void set_viewport(int width, int height);
    void set_clear_color(float red, float green, float blue);
    void set_scene_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void set_skybox(std::shared_ptr<Texture3D> texture);
    void set_depth_map_framebuffer(int size);

    hoverable::Id get_hovered_id() { return hovered_id; }
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
    void clear(int buffers);
    void draw_screen_quad(GLuint texture, std::shared_ptr<Shader> shader);
    void draw_origin();
    void draw_skybox();
    void draw_model(const Model* model);
    void draw_models_to_depth_buffer();
    void draw_models_normal();
    void draw_models_with_outline();
    void _draw_models_with_outline(const std::vector<Model*>& submodels);
    void draw_quads();
    void setup_shadows();
    void check_hovered_id(int x, int y);
    void maybe_initialize_assets();

    struct Storage {
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

    // Collections of pointers to models and quads
    std::vector<Model*> models;
    std::vector<Model*> models_outline;
    std::vector<Model*> models_cast_shadow;
    std::vector<Quad*> quads;

    hoverable::Id hovered_id = hoverable::null;
    FramebufferReader<4> reader;

    int shadow_map_size = 4096;

    std::vector<PostProcessing> post_processings;

    std::string SHADOW_VERTEX_SHADER = "data/shaders/internal/shadow.vert";
    std::string SHADOW_FRAGMENT_SHADER = "data/shaders/internal/shadow.frag";
    std::string SCREEN_QUAD_VERTEX_SHADER = "data/shaders/internal/screen_quad.vert";
    std::string SCREEN_QUAD_FRAGMENT_SHADER = "data/shaders/internal/screen_quad.frag";
    std::string OUTLINE_VERTEX_SHADER = "data/shaders/internal/outline.vert";
    std::string OUTLINE_FRAGMENT_SHADER = "data/shaders/internal/outline.frag";
    std::string SKYBOX_VERTEX_SHADER = "data/shaders/internal/skybox.vert";
    std::string SKYBOX_FRAGMENT_SHADER = "data/shaders/internal/skybox.frag";
    std::string QUAD3D_VERTEX_SHADER = "data/shaders/internal/quad3d.vert";
    std::string QUAD3D_FRAGMENT_SHADER = "data/shaders/internal/quad3d.frag";
#ifdef PLATFORM_GAME_DEBUG
    std::string ORIGIN_VERTEX_SHADER = "data/shaders/internal/origin.vert";
    std::string ORIGIN_FRAGMENT_SHADER = "data/shaders/internal/origin.frag";
#endif

    // Reference to application
    Application* app = nullptr;

    friend class Application;
};
