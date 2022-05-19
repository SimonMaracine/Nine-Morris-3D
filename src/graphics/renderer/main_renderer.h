#pragma once

#include <array>
#include <memory>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "application/platform.h"
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

    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    enum {
        NoLighting = 1 << 0,
        WithOutline = 1 << 1,
        CastShadow = 1 << 2,
        HasShadow = 1 << 3,
    };

    Renderer(Application* app);
    ~Renderer();

    void render();

    void add_model(Model& model, int options = 0);
    void remove_model(unsigned int handle);
    void update_model(Model& model, int options = 0);

    // unsigned int add_instancing_group();
    // void remove_instancing_group();
    // unsigned int add_instanced(unsigned int group_handle);
    // void remove_instanced(unsigned int group_handle, unsigned int handle);

    void clear_models();

    void set_viewport(int width, int height);
    void set_clear_color(float red, float green, float blue);
    void set_scene_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void set_skybox(std::shared_ptr<Texture3D> texture);
    void set_depth_map_framebuffer(int size);

    hoverable::Id get_hovered_id() { return hovered_id; }
    UniformBlockSpecification& get_projection_view_uniform_block() { return storage.projection_view_uniform_block; }
    UniformBlockSpecification& get_light_uniform_block() { return storage.light_uniform_block; }
    UniformBlockSpecification& get_light_space_uniform_block() { return storage.light_space_uniform_block; }
    std::shared_ptr<Framebuffer> get_scene_framebuffer() { return storage.scene_framebuffer; }

#ifdef NINE_MORRIS_3D_DEBUG
    std::shared_ptr<Shader> get_origin_shader() { return storage.origin_shader; }
#endif
    std::shared_ptr<Shader> get_outline_shader() { return storage.outline_shader; }
    std::shared_ptr<Shader> get_quad3d_shader() { return storage.quad3d_shader; }
    std::shared_ptr<Shader> get_screen_quad_shader() { return storage.screen_quad_shader; }
    std::shared_ptr<Shader> get_shadow_shader() { return storage.shadow_shader; }
    std::shared_ptr<Shader> get_skybox_shader() { return storage.skybox_shader; }

    DirectionalLight light;
    bool origin = false;  // This does nothing in release mode
private:
    void clear(int buffers);
    void draw_screen_quad(GLuint texture);
    void draw_origin();
    void draw_skybox();
    void setup_shadows();
    void check_hovered_id(int x, int y);

    struct Storage {
        std::shared_ptr<UniformBuffer> projection_view_uniform_buffer;
        std::shared_ptr<UniformBuffer> light_uniform_buffer;
        std::shared_ptr<UniformBuffer> light_space_uniform_buffer;

        UniformBlockSpecification projection_view_uniform_block;
        UniformBlockSpecification light_uniform_block;
        UniformBlockSpecification light_space_uniform_block;

        std::shared_ptr<Shader> skybox_shader;
        std::shared_ptr<Shader> screen_quad_shader;
        std::shared_ptr<Shader> quad3d_shader;
        std::shared_ptr<Shader> shadow_shader;
        std::shared_ptr<Shader> outline_shader;
#ifdef NINE_MORRIS_3D_DEBUG
        std::shared_ptr<Shader> origin_shader;
#endif

        std::shared_ptr<VertexArray> skybox_vertex_array;
        std::shared_ptr<VertexArray> screen_quad_vertex_array;
#ifdef NINE_MORRIS_3D_DEBUG
        std::shared_ptr<VertexArray> origin_vertex_array;
#endif

        std::shared_ptr<Texture3D> skybox_texture;
#ifdef NINE_MORRIS_3D_DEBUG
        std::shared_ptr<Texture> light_bulb_texture;
#endif

        std::shared_ptr<Framebuffer> scene_framebuffer;
        std::shared_ptr<Framebuffer> depth_map_framebuffer;
        std::shared_ptr<Framebuffer> intermediate_framebuffer;

        std::array<std::shared_ptr<PixelBuffer>, 4> pixel_buffers;
    } storage;

    // Ordered maps of pointers to models
    std::map<unsigned int, Model*> models;
    std::map<unsigned int, Model*> models_no_lighting;
    std::map<unsigned int, Model*> models_outline;
    std::map<unsigned int, Model*> models_cast_shadow;
    std::map<unsigned int, Model*> models_has_shadow;

    hoverable::Id hovered_id = hoverable::null;
    FramebufferReader<4> reader;

    int shadow_map_size = 4096;

    const char* SHADOW_VERTEX_SHADER = "data/shaders/internal/shadow.vert";
    const char* SHADOW_FRAGMENT_SHADER = "data/shaders/internal/shadow.frag";
    const char* SCREEN_QUAD_VERTEX_SHADER = "data/shaders/internal/screen_quad.vert";
    const char* SCREEN_QUAD_FRAGMENT_SHADER = "data/shaders/internal/screen_quad.frag";
    const char* OUTLINE_VERTEX_SHADER = "data/shaders/internal/outline.vert";
    const char* OUTLINE_FRAGMENT_SHADER = "data/shaders/internal/outline.frag";
    const char* SKYBOX_VERTEX_SHADER = "data/shaders/internal/skybox.vert";
    const char* SKYBOX_FRAGMENT_SHADER = "data/shaders/internal/skybox.frag";
    const char* QUAD3D_VERTEX_SHADER = "data/shaders/internal/quad3d.vert";
    const char* QUAD3D_FRAGMENT_SHADER = "data/shaders/internal/quad3d.frag";
#ifdef NINE_MORRIS_3D_DEBUG
    const char* ORIGIN_VERTEX_SHADER = "data/shaders/internal/origin.vert";
    const char* ORIGIN_FRAGMENT_SHADER = "data/shaders/internal/origin.frag";
#endif

    // Cache for the projection-view matrix
    glm::mat4 projection_view_matrix = glm::mat4(1.0f);

    // Reference to application
    Application* app = nullptr;

    friend class Application;
};
