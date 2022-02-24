#pragma once

#include <array>
#include <string>
#include <memory>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "application/platform.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/framebuffer.h"
#include "graphics/renderer/font.h"
#include "graphics/renderer/camera.h"
#include "graphics/renderer/material.h"
#include "graphics/renderer/light.h"

class Application;

class Renderer {
public:
    class Model {
    public:
        std::shared_ptr<VertexArray> vertex_array;
        int index_count = 0;
        // std::shared_ptr<Shader> shader;
        std::shared_ptr<MaterialInstance> material;
        // std::shared_ptr<Texture> diffuse_texture;
        // std::shared_ptr<Texture> normal_texture;

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        float scale = 1.0f;

        // glm::vec3 tint_color = glm::vec3(0.0f);
        // glm::vec3 outline_color = glm::vec3(0.0f);

        // glm::vec3 specular_color = glm::vec3(0.0f);
        // float shininess = 0.0f;
    private:
        unsigned int id = 0;  // TODO Don't know if needed

        friend class Renderer;
    };

    enum {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    Renderer(Application* app);
    ~Renderer();

    void render();

    unsigned int add_model(Model& model, bool with_tint_color, bool with_outline, bool with_shadows);
    void remove_model(unsigned int handle);

    // unsigned int add_instancing_group();
    // void remove_instancing_group();
    // unsigned int add_instanced(unsigned int group_handle);
    // void remove_instanced(unsigned int group_handle, unsigned int handle);

    void set_viewport(int width, int height);
    void set_clear_color(float red, float green, float blue);
    void set_scene_framebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void set_skybox(std::shared_ptr<Texture3D> texture);

    DirectionalLight light;
private:
    void clear(int buffers);
    void draw_screen_quad(GLuint texture);
    void draw_origin();
    void draw_skybox();

    struct Storage {
        std::shared_ptr<Shader> skybox_shader;
        std::shared_ptr<Shader> quad2d_shader;
        std::shared_ptr<Shader> screen_quad_shader;
        std::shared_ptr<Shader> quad3d_shader;
        std::shared_ptr<Shader> shadow_shader;
        std::shared_ptr<Shader> outline_shader;
        std::shared_ptr<Shader> text_shader;
#ifdef NINE_MORRIS_3D_DEBUG
        std::shared_ptr<Shader> origin_shader;
#endif

        std::shared_ptr<VertexArray> skybox_vertex_array;
        std::shared_ptr<VertexArray> quad2d_vertex_array;
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

        glm::mat4 orthographic_projection_matrix = glm::mat4(1.0f);
    } storage;

    // Ordered maps of pointers to models
    std::map<unsigned int, Model*> models;
    std::map<unsigned int, Model*> models_tint_color;
    std::map<unsigned int, Model*> models_outline;
    std::map<unsigned int, Model*> models_shadow;

    const char* SHADOW_VERTEX_SHADER = "data/shaders/internal/shadow.vert";
    const char* SHADOW_FRAGMENT_SHADER = "data/shaders/internal/shadow.frag";
    const char* SCREEN_QUAD_VERTEX_SHADER = "data/shaders/internal/screen_quad.vert";
    const char* SCREEN_QUAD_FRAGMENT_SHADER = "data/shaders/internal/screen_quad.frag";
    const char* OUTLINE_VERTEX_SHADER = "data/shaders/internal/outline.vert";
    const char* OUTLINE_FRAGMENT_SHADER = "data/shaders/internal/outline.frag";
    const char* SKYBOX_VERTEX_SHADER = "data/shaders/internal/skybox.vert";
    const char* SKYBOX_FRAGMENT_SHADER = "data/shaders/internal/skybox.frag";
    const char* QUAD2D_VERTEX_SHADER = "data/shaders/internal/quad2d.vert";
    const char* QUAD2D_FRAGMENT_SHADER = "data/shaders/internal/quad2d.frag";
    const char* QUAD3D_VERTEX_SHADER = "data/shaders/internal/quad3d.vert";
    const char* QUAD3D_FRAGMENT_SHADER = "data/shaders/internal/quad3d.frag";
    const char* TEXT_VERTEX_SHADER = "data/shaders/internal/text.vert";
    const char* TEXT_FRAGMENT_SHADER = "data/shaders/internal/text.frag";
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
