#pragma once

#include <glm/glm.hpp>

#include "engine/application/platform.h"
#include "engine/application/event.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/graphics/opengl/texture.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/framebuffer_reader.h"
#include "engine/graphics/font.h"
#include "engine/graphics/camera.h"
#include "engine/graphics/material.h"
#include "engine/graphics/light.h"
#include "engine/graphics/identifier.h"
#include "engine/graphics/post_processing.h"
#include "engine/other/camera_controller.h"
#include "engine/other/encrypt.h"
#include "engine/scene/renderables.h"
#include "engine/scene/scene_list.h"

class Application;

class Renderer {
public:
    struct Storage;

    Renderer(Application* app);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void render(const SceneList& scene);

    void add_post_processing(std::unique_ptr<PostProcessingStep>&& post_processing_step);

    void set_scene_framebuffer(int samples);
    void set_shadow_map_framebuffer(int size);
    void set_skybox(std::shared_ptr<gl::Texture3D> texture);
    void set_camera_controller(const CameraController* camera_controller);

    identifier::Id get_hovered_id() { return hovered_id; }
    PostProcessingContext& get_post_processing_context() { return post_processing_context; }
    const Storage& get_storage() { return storage; }

    bool origin = false;  // This does nothing in release mode
    DirectionalLight directional_light;

    struct LightSpace {
        float left = 0.0f;
        float right = 0.0f;
        float bottom = 0.0f;
        float top = 0.0f;
        float lens_near = 1.0f;
        float lens_far = 1.0f;
        float position_divisor = 1.0f;
    } light_space;
private:
    struct IdMatrix {
        float id;
        glm::mat4 model_matrix;
    };

    void draw_screen_quad(GLuint texture);
    void post_processing();
    void end_rendering();

    void draw_origin();
    void draw_skybox();
    void draw_model(const renderables::Model* model);
    void draw_model_with_outline(const renderables::Model* model);
    void draw_models(const SceneList& scene);
    void draw_models_with_outline(const SceneList& scene);
    void draw_models_to_depth_buffer(const SceneList& scene);
    void draw_quad(const renderables::Quad* quad);
    void draw_quads(const SceneList& scene);

    void add_bounding_box(const renderables::Model* model, std::vector<IdMatrix>& buffer_ids_transforms);
    void draw_bounding_boxes(const SceneList& scene);
    void setup_shadows();
    void setup_uniform_buffers();
    void validate_hovered_id(int x, int y);
    void cache_camera_data();
    bool on_window_resized(event::WindowResizedEvent&);

    void initialize_uniform_buffers();
    void initialize_skybox_renderer();
    void initialize_screen_quad_renderer();
    void initialize_quad3d_renderer();
    void initialize_shadow_renderer();
    void initialize_outline_renderer();
    void initialize_bounding_box_renderer();
    void initialize_origin_renderer();
    void initialize_framebuffers();
    void initialize_pixel_buffers();

    struct Storage {
        std::shared_ptr<gl::UniformBuffer> projection_view_uniform_buffer;
        std::shared_ptr<gl::UniformBuffer> light_uniform_buffer;
        std::shared_ptr<gl::UniformBuffer> light_view_uniform_buffer;
        std::shared_ptr<gl::UniformBuffer> light_space_uniform_buffer;

        gl::UniformBlockSpecification projection_view_uniform_block;
        gl::UniformBlockSpecification light_uniform_block;
        gl::UniformBlockSpecification light_view_uniform_block;
        gl::UniformBlockSpecification light_space_uniform_block;

        std::shared_ptr<gl::Shader> skybox_shader;
        std::shared_ptr<gl::Shader> screen_quad_shader;
        std::shared_ptr<gl::Shader> quad3d_shader;
        std::shared_ptr<gl::Shader> shadow_shader;
        std::shared_ptr<gl::Shader> outline_shader;
        std::shared_ptr<gl::Shader> box_shader;
        std::shared_ptr<gl::Shader> origin_shader;

        std::shared_ptr<gl::VertexArray> skybox_vertex_array;
        std::shared_ptr<gl::VertexBuffer> skybox_buffer;
        std::shared_ptr<gl::VertexArray> screen_quad_vertex_array;
        std::shared_ptr<gl::VertexBuffer> screen_quad_buffer;
        std::shared_ptr<gl::VertexArray> quad3d_vertex_array;
        std::shared_ptr<gl::VertexBuffer> quad3d_buffer;
        std::shared_ptr<gl::VertexArray> box_vertex_array;
        std::shared_ptr<gl::VertexBuffer> box_buffer;
        std::shared_ptr<gl::VertexBuffer> box_ids_transforms_buffer;
        std::shared_ptr<gl::IndexBuffer> box_index_buffer;
        std::shared_ptr<gl::VertexArray> origin_vertex_array;
        std::shared_ptr<gl::VertexBuffer> origin_buffer;

        std::shared_ptr<gl::Texture3D> skybox_texture;

        std::shared_ptr<gl::Framebuffer> scene_framebuffer;
        std::shared_ptr<gl::Framebuffer> intermediate_framebuffer;
        std::shared_ptr<gl::Framebuffer> shadow_map_framebuffer;
        std::shared_ptr<gl::Framebuffer> bounding_box_framebuffer;

        std::array<std::shared_ptr<gl::PixelBuffer>, 4> pixel_buffers;
    } storage;

    struct CameraCache {
        glm::mat4 projection_matrix = glm::mat4(1.0f);
        glm::mat4 view_matrix = glm::mat4(1.0f);
        glm::mat4 projection_view_matrix = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0.0f);
    } camera_cache;

    identifier::Id hovered_id = identifier::null;
    FramebufferReader<4> framebuffer_reader;
    const CameraController* camera_controller = nullptr;  // Don't use this directly

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
    const char* BOUNDING_BOX_VERTEX_SHADER = ENCR("engine_data/shaders/bounding_box.vert");
    const char* BOUNDING_BOX_FRAGMENT_SHADER = ENCR("engine_data/shaders/bounding_box.frag");
    const char* ORIGIN_VERTEX_SHADER = "engine_data/shaders/origin.vert";
    const char* ORIGIN_FRAGMENT_SHADER = "engine_data/shaders/origin.frag";

    Application* app = nullptr;

    friend class Application;
};
