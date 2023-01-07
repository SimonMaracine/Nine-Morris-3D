#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/application/application.h"
#include "engine/application/platform.h"
#include "engine/application/events.h"
#include "engine/application/input.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/framebuffer_reader.h"
#include "engine/graphics/post_processing.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/graphics/opengl/texture.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/other/file_system.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/other/camera_controller.h"

static constexpr int SHADOW_MAP_UNIT = 2;
static constexpr glm::vec3 CLEAR_COLOR = { 0.1f, 0.1f, 0.1f };
static constexpr int BOUNDING_BOX_DIVISOR = 4;

Renderer::Renderer(Application* app)
    : app(app) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, 1.0f);

    storage.projection_view_uniform_buffer = std::make_shared<gl::UniformBuffer>();
    storage.light_uniform_buffer = std::make_shared<gl::UniformBuffer>();
    storage.light_view_position_uniform_buffer = std::make_shared<gl::UniformBuffer>();
    storage.light_space_uniform_buffer = std::make_shared<gl::UniformBuffer>();

    storage.projection_view_uniform_block.block_name = "ProjectionView";
    storage.projection_view_uniform_block.field_count = 1;
    storage.projection_view_uniform_block.field_names = { "u_projection_view_matrix" };
    storage.projection_view_uniform_block.uniform_buffer = storage.projection_view_uniform_buffer;
    storage.projection_view_uniform_block.binding_index = 0;

    storage.light_uniform_block.block_name = "Light";
    storage.light_uniform_block.field_count = 3;
    storage.light_uniform_block.field_names = {
        "u_light_ambient",
        "u_light_diffuse",
        "u_light_specular"
    };
    storage.light_uniform_block.uniform_buffer = storage.light_uniform_buffer;
    storage.light_uniform_block.binding_index = 1;

    storage.light_view_position_uniform_block.block_name = "LightViewPosition";
    storage.light_view_position_uniform_block.field_count = 2;
    storage.light_view_position_uniform_block.field_names = {
        "u_light_position",
        "u_view_position"
    };
    storage.light_view_position_uniform_block.uniform_buffer = storage.light_view_position_uniform_buffer;
    storage.light_view_position_uniform_block.binding_index = 2;

    storage.light_space_uniform_block.block_name = "LightSpace";
    storage.light_space_uniform_block.field_count = 1;
    storage.light_space_uniform_block.field_names = { "u_light_space_matrix" };
    storage.light_space_uniform_block.uniform_buffer = storage.light_space_uniform_buffer;
    storage.light_space_uniform_block.binding_index = 3;

    using namespace encrypt;

    {
        storage.skybox_shader = std::make_shared<gl::Shader>(
            encr(file_system::path_for_assets(SKYBOX_VERTEX_SHADER)),
            encr(file_system::path_for_assets(SKYBOX_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_projection_view_matrix", "u_skybox" }
        );
    }

    {
        storage.screen_quad_shader = std::make_shared<gl::Shader>(
            encr(file_system::path_for_assets(SCREEN_QUAD_VERTEX_SHADER)),
            encr(file_system::path_for_assets(SCREEN_QUAD_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );
    }

    {
        storage.quad3d_shader = std::make_shared<gl::Shader>(
            encr(file_system::path_for_assets(QUAD3D_VERTEX_SHADER)),
            encr(file_system::path_for_assets(QUAD3D_FRAGMENT_SHADER)),
            std::vector<std::string> {
                "u_model_matrix",
                "u_view_matrix",
                "u_projection_matrix",
                "u_texture"
            }
        );
    }

    {
        storage.shadow_shader = std::make_shared<gl::Shader>(
            encr(file_system::path_for_assets(SHADOW_VERTEX_SHADER)),
            encr(file_system::path_for_assets(SHADOW_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_model_matrix" },
            std::initializer_list { storage.light_space_uniform_block }
        );
    }

    {
        storage.outline_shader = std::make_shared<gl::Shader>(
            encr(file_system::path_for_assets(OUTLINE_VERTEX_SHADER)),
            encr(file_system::path_for_assets(OUTLINE_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_model_matrix", "u_color" },
            std::initializer_list { storage.projection_view_uniform_block }
        );
    }

    {
        storage.box_shader = std::make_shared<gl::Shader>(
            encr(file_system::path_for_assets(BOUNDING_BOX_VERTEX_SHADER)),
            encr(file_system::path_for_assets(BOUNDING_BOX_FRAGMENT_SHADER)),
            std::vector<std::string> {},
            std::initializer_list { storage.projection_view_uniform_block }
        );
    }

#ifdef NM3D_PLATFORM_DEBUG
    {
        storage.origin_shader = std::make_shared<gl::Shader>(
            file_system::path_for_assets(ORIGIN_VERTEX_SHADER),
            file_system::path_for_assets(ORIGIN_FRAGMENT_SHADER),
            std::vector<std::string> { "u_projection_view_matrix" }
        );
    }
#endif

    {
        storage.skybox_buffer = std::make_shared<gl::Buffer>(gl::CUBEMAP_VERTICES, sizeof(gl::CUBEMAP_VERTICES));
        BufferLayout layout;
        layout.add(0, BufferLayout::Float, 3);
        storage.skybox_vertex_array = std::make_shared<gl::VertexArray>();
        storage.skybox_vertex_array->add_buffer(storage.skybox_buffer, layout);
        gl::VertexArray::unbind();
    }

    {
        static constexpr float screen_quad_vertices[] = {
            -1.0f,  1.0f,
            -1.0f, -1.0f,
             1.0f,  1.0f,
             1.0f,  1.0f,
            -1.0f, -1.0f,
             1.0f, -1.0f,
        };

        storage.screen_quad_buffer = std::make_shared<gl::Buffer>(screen_quad_vertices, sizeof(screen_quad_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Float, 2);
        storage.screen_quad_vertex_array = std::make_shared<gl::VertexArray>();
        storage.screen_quad_vertex_array->add_buffer(storage.screen_quad_buffer, layout);
        gl::VertexArray::unbind();
    }

    {
        static constexpr float quad_vertices[] = {
            -1.0f,  1.0f,    0.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
             1.0f,  1.0f,    1.0f, 1.0f,
             1.0f,  1.0f,    1.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
             1.0f, -1.0f,    1.0f, 0.0f
        };

        storage.quad_buffer = std::make_shared<gl::Buffer>(quad_vertices, sizeof(quad_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Float, 2);
        layout.add(1, BufferLayout::Float, 2);
        storage.quad_vertex_array = std::make_shared<gl::VertexArray>();
        storage.quad_vertex_array->add_buffer(storage.quad_buffer, layout);
        gl::VertexArray::unbind();
    }

    {
        static constexpr float box_vertices[] = {
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f
        };

        static constexpr unsigned int box_indices[] = {
            0, 1, 2, 0, 2, 3,
            1, 5, 6, 1, 6, 2,
            5, 4, 6, 4, 7, 6,
            4, 0, 7, 0, 3, 7,
            3, 2, 6, 3, 6, 7,
            4, 5, 1, 4, 1, 0
        };

        storage.box_buffer = std::make_shared<gl::Buffer>(box_vertices, sizeof(box_vertices));
        storage.box_ids = std::make_shared<gl::Buffer>(1, gl::DrawHint::Stream);
        storage.box_transforms = std::make_shared<gl::Buffer>(1, gl::DrawHint::Stream);
        storage.box_index_buffer = std::make_shared<gl::IndexBuffer>(box_indices, sizeof(box_indices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Float, 3);
        BufferLayout layout_ids;
        layout_ids.add(1, BufferLayout::Float, 1, true);
        BufferLayout layout_transforms;
        layout_transforms.add(2, BufferLayout::Float, 4, true);
        layout_transforms.add(3, BufferLayout::Float, 4, true);
        layout_transforms.add(4, BufferLayout::Float, 4, true);
        layout_transforms.add(5, BufferLayout::Float, 4, true);
        storage.box_vertex_array = std::make_shared<gl::VertexArray>();
        storage.box_vertex_array->add_buffer(storage.box_buffer, layout);
        storage.box_vertex_array->add_buffer(storage.box_ids, layout_ids);
        storage.box_vertex_array->add_buffer(storage.box_transforms, layout_transforms);
        storage.box_vertex_array->add_index_buffer(storage.box_index_buffer);
        gl::VertexArray::unbind();
    }

#ifdef NM3D_PLATFORM_DEBUG
    {
        static constexpr float origin_vertices[] = {
            -20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
             20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
              0.0f, -20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
              0.0f,  20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
              0.0f,   0.0f, -20.0f,    0.0f, 0.0f, 1.0f,
              0.0f,   0.0f,  20.0f,    0.0f, 0.0f, 1.0f
        };

        storage.origin_buffer = std::make_shared<gl::Buffer>(origin_vertices, sizeof(origin_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Float, 3);
        layout.add(1, BufferLayout::Float, 3);
        storage.origin_vertex_array = std::make_shared<gl::VertexArray>();
        storage.origin_vertex_array->add_buffer(storage.origin_buffer, layout);
        gl::VertexArray::unbind();
    }
#endif

    {
        gl::FramebufferSpecification specification;
        specification.width = app->data().width;
        specification.height = app->data().height;
        specification.color_attachments = {
            gl::Attachment {gl::AttachmentFormat::RGBA8, gl::AttachmentType::Texture}
        };

        storage.intermediate_framebuffer = std::make_shared<gl::Framebuffer>(specification);

        app->purge_framebuffers();
        app->add_framebuffer(storage.intermediate_framebuffer);
    }

    {
        gl::FramebufferSpecification specification;
        specification.width = app->data().width / BOUNDING_BOX_DIVISOR;
        specification.height = app->data().height / BOUNDING_BOX_DIVISOR;
        specification.resize_divisor = BOUNDING_BOX_DIVISOR;
        specification.color_attachments = {
            gl::Attachment {gl::AttachmentFormat::RED_FLOAT, gl::AttachmentType::Renderbuffer}
        };
        specification.depth_attachment = gl::Attachment {
            gl::AttachmentFormat::DEPTH32, gl::AttachmentType::Renderbuffer
        };
        static constexpr float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };  // TODO right now not used
        specification.clear_drawbuffer = 0;
        specification.clear_value = color;

        storage.bounding_box_framebuffer = std::make_shared<gl::Framebuffer>(specification);

        app->purge_framebuffers();
        app->add_framebuffer(storage.bounding_box_framebuffer);
    }

    storage.pixel_buffers = {
        std::make_shared<gl::PixelBuffer>(sizeof(float)),
        std::make_shared<gl::PixelBuffer>(sizeof(float)),
        std::make_shared<gl::PixelBuffer>(sizeof(float)),
        std::make_shared<gl::PixelBuffer>(sizeof(float))
    };

    framebuffer_reader = FramebufferReader<4> {storage.pixel_buffers, storage.bounding_box_framebuffer};

    initialize_uniform_variables();

    // Setup events
    app->evt.add_event<WindowResizedEvent, &Renderer::on_window_resized>(this);

    DEB_INFO("Initialized renderer");
}

Renderer::~Renderer() {
    DEB_INFO("Uninitialized renderer");
}

void Renderer::render() {
    cache_camera_data();
    setup_uniform_buffers();

    setup_shadows();
    storage.shadow_map_framebuffer->bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    const auto& shadow_map_specification = storage.shadow_map_framebuffer->get_specification();
    glViewport(0, 0, shadow_map_specification.width, shadow_map_specification.height);

    // Render objects with shadows to depth buffer
    draw_models_to_depth_buffer();

    storage.scene_framebuffer->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    const auto& scene_specification = storage.scene_framebuffer->get_specification();
    glViewport(0, 0, scene_specification.width, scene_specification.height);

    // Bind shadow map for use in shadow rendering
    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_UNIT);
    glBindTexture(GL_TEXTURE_2D, storage.shadow_map_framebuffer->get_depth_attachment());

    // Set to zero, because we are also rendering objects with outline later
    glStencilMask(0x00);

    // Render all normal models
    draw_models();

    // Render all models with outline
    draw_models_with_outline();

#ifdef NM3D_PLATFORM_DEBUG
    if (origin) {
        draw_origin();
    }
#endif

    // Render the skybox last (but before quads)
    if (storage.skybox_texture != nullptr) {
        draw_skybox();
    }

    // Render 3D quads
    draw_quads();

    // Blit the resulted scene texture to an intermediate texture, resolving anti-aliasing
    storage.scene_framebuffer->blit(
        storage.intermediate_framebuffer.get(), app->data().width, app->data().height
    );

    storage.bounding_box_framebuffer->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const auto& bounding_box_specification = storage.bounding_box_framebuffer->get_specification();
    glViewport(0, 0, bounding_box_specification.width, bounding_box_specification.height);

    // Render bounding boxes for models that are pickable
    draw_bounding_boxes();

    // Read the texture for mouse picking
    const auto [mouse_x, mouse_y] = input::get_mouse();
    const int x = static_cast<int>(mouse_x) / BOUNDING_BOX_DIVISOR;
    const int y = (app->data().height - static_cast<int>(mouse_y)) / BOUNDING_BOX_DIVISOR;
    framebuffer_reader.read(0, x, y);

    storage.intermediate_framebuffer->bind();

    const auto& intermediate_specification = storage.intermediate_framebuffer->get_specification();
    glViewport(0, 0, intermediate_specification.width, intermediate_specification.height);

    // Do post processing and render the final image to the screen
    end_rendering();

    float* data;
    framebuffer_reader.get<float>(&data);
    hovered_id = *data;

    validate_hovered_id(mouse_x, mouse_y);
}

void Renderer::add_model(std::shared_ptr<Model> model) {
    auto iter = std::find(models.begin(), models.end(), model);

    if (iter == models.end()) {
        models.push_back(model);
    } else {
        DEB_WARNING("Model already present in list");
    }
}

void Renderer::remove_model(std::shared_ptr<Model> model) {
    auto iter = std::find(models.begin(), models.end(), model);

    if (iter != models.end()) {
        models.erase(iter);
    }
}

void Renderer::add_quad(std::shared_ptr<Quad> quad) {
    auto iter = std::find(quads.begin(), quads.end(), quad);

    if (iter == quads.end()) {
        quads.push_back(quad);
    } else {
        DEB_WARNING("Quad already present in list");
    }
}

void Renderer::remove_quad(std::shared_ptr<Quad> quad) {
    auto iter = std::find(quads.begin(), quads.end(), quad);

    if (iter != quads.end()) {
        quads.erase(iter);
    }
}

void Renderer::clear() {
    models.clear();
    quads.clear();
}

void Renderer::setup_shader(std::shared_ptr<gl::Shader> shader) {
    const auto& uniforms = shader->get_uniforms();

    if (std::find(uniforms.begin(), uniforms.end(), "u_shadow_map") != uniforms.end()) {
        shader->bind();
        shader->upload_uniform_int("u_shadow_map", SHADOW_MAP_UNIT);
        gl::Shader::unbind();
    }
}

void Renderer::add_post_processing(std::unique_ptr<PostProcessingStep>&& post_processing_step) {
    post_processing_step->prepare(post_processing_context);
    post_processing_context.steps.push_back(std::move(post_processing_step));
}

void Renderer::set_scene_framebuffer(int samples) {
    ASSERT(samples > 0, "Samples must be greater than 0");

    gl::FramebufferSpecification specification;
    specification.width = app->data().width;
    specification.height = app->data().height;
    specification.samples = samples;
    specification.color_attachments = {
        gl::Attachment {gl::AttachmentFormat::RGBA8, gl::AttachmentType::Renderbuffer}
    };
    specification.depth_attachment = gl::Attachment {
        gl::AttachmentFormat::DEPTH24_STENCIL8, gl::AttachmentType::Renderbuffer
    };

    storage.scene_framebuffer = std::make_shared<gl::Framebuffer>(specification);

    app->purge_framebuffers();
    app->add_framebuffer(storage.scene_framebuffer);
}

void Renderer::set_shadow_map_framebuffer(int size) {
    ASSERT(size > 0, "Shadow map size must be greater than 0");

    shadow_map_size = size;

    gl::FramebufferSpecification specification;
    specification.width = shadow_map_size;
    specification.height = shadow_map_size;
    specification.depth_attachment = gl::Attachment {gl::AttachmentFormat::DEPTH32, gl::AttachmentType::Texture};
    specification.white_border_for_depth_texture = true;
    specification.resizable = false;

    storage.shadow_map_framebuffer = std::make_shared<gl::Framebuffer>(specification);

    app->purge_framebuffers();
    app->add_framebuffer(storage.shadow_map_framebuffer);
}

void Renderer::set_skybox(std::shared_ptr<gl::Texture3D> texture) {
    storage.skybox_texture = texture;
}

void Renderer::set_camera_controller(const CameraController* camera_controller) {
    this->camera_controller = camera_controller;

    if (camera_controller != nullptr) {
        cache_camera_data();
    } else {
        camera_cache = CameraCache {};
    }

    // Update the projection
    on_window_resized(WindowResizedEvent {0, 0});
}

void Renderer::draw_screen_quad(GLuint texture) {
    storage.screen_quad_shader->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::post_processing() {
    post_processing_context.original_texture = storage.intermediate_framebuffer->get_color_attachment(0);
    post_processing_context.last_texture = post_processing_context.original_texture;
    post_processing_context.textures.clear();

    for (size_t i = 0; i < post_processing_context.steps.size(); i++) {
        const PostProcessingStep* step = post_processing_context.steps[i].get();
        const gl::FramebufferSpecification& specification = step->framebuffer->get_specification();

        step->framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, specification.width, specification.height);

        step->render(post_processing_context);

        glViewport(0, 0, app->data().width, app->data().height);

        post_processing_context.last_texture = step->framebuffer->get_color_attachment(0);
        post_processing_context.textures.push_back(post_processing_context.last_texture);
    }
}

void Renderer::end_rendering() {
    storage.screen_quad_vertex_array->bind();

    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    post_processing();

    // Draw the final result to the screen
    gl::Framebuffer::bind_default();
    glClear(GL_COLOR_BUFFER_BIT);
    draw_screen_quad(post_processing_context.last_texture);
    glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

#ifdef NM3D_PLATFORM_DEBUG
void Renderer::draw_origin() {
    storage.origin_shader->bind();
    storage.origin_shader->upload_uniform_mat4("u_projection_view_matrix", camera_cache.projection_view_matrix);

    storage.origin_vertex_array->bind();

    glDrawArrays(GL_LINES, 0, 6);
}
#endif

void Renderer::draw_skybox() {
    const glm::mat4& projection = camera_cache.projection_matrix;
    const glm::mat4 view = glm::mat4(glm::mat3(camera_cache.view_matrix));

    storage.skybox_shader->bind();
    storage.skybox_shader->upload_uniform_mat4("u_projection_view_matrix", projection * view);

    storage.skybox_vertex_array->bind();
    storage.skybox_texture->bind(0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::draw_model(const Model* model) {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, model->position);
    matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, glm::vec3(model->scale));

    model->vertex_array->bind();
    model->material->bind();

    model->material->get_shader()->upload_uniform_mat4("u_model_matrix", matrix);

    glDrawElements(GL_TRIANGLES, model->index_buffer->get_index_count(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::draw_model_with_outline(const Model* model) {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    draw_model(model);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    {
        constexpr float SIZE = 3.6f;

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale + SIZE));

        storage.outline_shader->bind();
        storage.outline_shader->upload_uniform_mat4("u_model_matrix", matrix);
        storage.outline_shader->upload_uniform_vec3("u_color", model->outline_color.value());  // Should never throw

        glDrawElements(GL_TRIANGLES, model->index_buffer->get_index_count(), GL_UNSIGNED_INT, nullptr);
    }

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void Renderer::draw_models() {
    for (size_t i = 0; i < models.size(); i++) {
        const Model* model = models[i].get();

        if (model->outline_color.has_value()) {
            continue;  // This model is rendered differently
        }

        draw_model(model);
    }
}

void Renderer::draw_models_with_outline() {
    static std::vector<const Model*> outline_models;

    outline_models.clear();

    std::for_each(models.begin(), models.end(), [](const std::shared_ptr<Model>& model) {
        if (model->outline_color.has_value()) {
            outline_models.push_back(model.get());
        }
    });

    std::sort(outline_models.begin(), outline_models.end(), [this](const Model* lhs, const Model* rhs) {
        const float distance1 = glm::distance(lhs->position, camera_cache.position);
        const float distance2 = glm::distance(rhs->position, camera_cache.position);

        return distance1 < distance2;
    });

    for (const Model* model : outline_models) {
        draw_model_with_outline(model);
    }
}

void Renderer::draw_models_to_depth_buffer() {
    storage.shadow_shader->bind();

    for (size_t i = 0; i < models.size(); i++) {
        const Model* model = models[i].get();

        if (model->cast_shadow) {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, model->position);
            matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(model->scale));

            storage.shadow_shader->upload_uniform_mat4("u_model_matrix", matrix);

            model->vertex_array->bind();

            glDrawElements(GL_TRIANGLES, model->index_buffer->get_index_count(), GL_UNSIGNED_INT, nullptr);
        }
    }
}

void Renderer::draw_quad(const Quad* quad) {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, quad->position);
    matrix = glm::scale(matrix, glm::vec3(quad->scale));

    storage.quad3d_shader->upload_uniform_mat4("u_model_matrix", matrix);

    quad->texture->bind(0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::draw_quads() {
    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_mat4("u_view_matrix", camera_cache.view_matrix);

    storage.quad_vertex_array->bind();

    std::sort(quads.begin(), quads.end(), [this](const std::shared_ptr<Quad>& lhs, const std::shared_ptr<Quad>& rhs) {
        const float distance1 = glm::distance(lhs->position, camera_cache.position);
        const float distance2 = glm::distance(rhs->position, camera_cache.position);

        return distance1 > distance2;
    });

    for (size_t i = 0; i < quads.size(); i++) {
        const Quad* quad = quads[i].get();

        draw_quad(quad);
    }
}

void Renderer::prepare_bounding_box(const Model* model, std::vector<float>& buffer_ids, std::vector<glm::mat4>& buffer_transforms) {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, model->position);
    matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, model->bounding_box->size);

    buffer_ids.push_back(model->bounding_box->id);
    buffer_transforms.push_back(matrix);
}

void Renderer::draw_bounding_boxes() {
    storage.box_shader->bind();
    storage.box_vertex_array->bind();

    static std::vector<const Model*> bounding_box_models;
    static std::vector<const Model*> bounding_box_models_unsorted;
    static std::vector<float> buffer_ids;
    static std::vector<glm::mat4> buffer_transforms;

    bounding_box_models.clear();
    bounding_box_models_unsorted.clear();
    buffer_ids.clear();
    buffer_transforms.clear();

    std::for_each(models.begin(), models.end(), [](const std::shared_ptr<Model>& model) {
        if (model->bounding_box.has_value()) {
            if (model->bounding_box->sort) {
                bounding_box_models.push_back(model.get());
            } else {
                bounding_box_models_unsorted.push_back(model.get());
            }
        }
    });

    std::sort(bounding_box_models.begin(), bounding_box_models.end(), [this](const Model* lhs, const Model* rhs) {
        const float distance1 = glm::distance(lhs->position, camera_cache.position);
        const float distance2 = glm::distance(rhs->position, camera_cache.position);

        return distance1 > distance2;
    });

    for (const Model* model : bounding_box_models_unsorted) {
        prepare_bounding_box(model, buffer_ids, buffer_transforms);
    }

    for (const Model* model : bounding_box_models) {
        prepare_bounding_box(model, buffer_ids, buffer_transforms);
    }

    storage.box_ids->bind();
    storage.box_ids->update_data(buffer_ids.data(), sizeof(float) * buffer_ids.size());  // TODO optimize this
    storage.box_transforms->bind();
    storage.box_transforms->update_data(buffer_transforms.data(), sizeof(glm::mat4) * buffer_transforms.size());

    // Disable blending, because this is a floating-point framebuffer
    glDisable(GL_BLEND);

    glDrawElementsInstanced(
        GL_TRIANGLES,
        storage.box_index_buffer->get_index_count(),
        GL_UNSIGNED_INT,
        nullptr,
        bounding_box_models.size() + bounding_box_models_unsorted.size()
    );

    glEnable(GL_BLEND);
}

void Renderer::setup_shadows() {
    const glm::mat4 projection = glm::ortho(
        light_space.left,
        light_space.right,
        light_space.bottom,
        light_space.top,
        light_space.lens_near,
        light_space.lens_far
    );
    const glm::mat4 view = glm::lookAt(
        light.position / light_space.light_divisor,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    const glm::mat4 light_space_matrix = projection * view;

    // Should be already configured
    storage.light_space_uniform_buffer->set(&light_space_matrix, 0);
    storage.light_space_uniform_buffer->bind();
    storage.light_space_uniform_buffer->upload_data();
}

void Renderer::setup_uniform_buffers() {
    // Should be already configured
    storage.projection_view_uniform_buffer->set(&camera_cache.projection_view_matrix, 0);
    storage.projection_view_uniform_buffer->bind();
    storage.projection_view_uniform_buffer->upload_data();

    if (storage.light_uniform_buffer->is_configured()) {
        storage.light_uniform_buffer->set(&light.ambient_color, 0);
        storage.light_uniform_buffer->set(&light.diffuse_color, 1);
        storage.light_uniform_buffer->set(&light.specular_color, 2);
        storage.light_uniform_buffer->bind();
        storage.light_uniform_buffer->upload_data();
    }

    if (storage.light_view_position_uniform_buffer->is_configured()) {
        storage.light_view_position_uniform_buffer->set(&light.position, 0);
        storage.light_view_position_uniform_buffer->set(&camera_cache.position, 1);
        storage.light_view_position_uniform_buffer->bind();
        storage.light_view_position_uniform_buffer->upload_data();
    }
}

void Renderer::validate_hovered_id(int x, int y) {
    if (x > app->data().width || x < 0 || y > app->data().height || y < 0) {
        hovered_id = identifier::null;
    }
}

void Renderer::cache_camera_data() {
    if (camera_controller != nullptr) {
        camera_cache.projection_matrix = camera_controller->get_camera().get_projection_matrix();
        camera_cache.view_matrix = camera_controller->get_camera().get_view_matrix();
        camera_cache.projection_view_matrix = camera_controller->get_camera().get_projection_view_matrix();
        camera_cache.position = camera_controller->get_position();
    }
}

void Renderer::initialize_uniform_variables() {
    storage.screen_quad_shader->bind();
    storage.screen_quad_shader->upload_uniform_int("u_screen_texture", 0);

    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_int("u_texture", 0);
    storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix", camera_cache.projection_matrix);

    gl::Shader::unbind();
}

void Renderer::on_window_resized(const WindowResizedEvent&) {
    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix", camera_cache.projection_matrix);
}

namespace render_helpers {
    void clear(int buffers) {
        glClear(buffers);
    }

    void viewport(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void clear_color(float red, float green, float blue) {
        glClearColor(red, green, blue, 1.0f);
    }

    void bind_texture_2d(GLuint texture, int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void draw_arrays(int count) {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }
}
