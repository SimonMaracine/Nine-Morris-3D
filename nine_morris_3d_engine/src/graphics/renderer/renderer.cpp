#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/application_base/application.h"
#include "engine/application_base/platform.h"
#include "engine/application_base/events.h"
#include "engine/application_base/input.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/graphics/opengl/texture.h"
#include "engine/graphics/opengl/framebuffer.h"
#include "engine/graphics/renderer/renderer.h"
#include "engine/graphics/renderer/render_helpers.h"
#include "engine/graphics/framebuffer_reader.h"
#include "engine/graphics/post_processing.h"
#include "engine/other/file_system.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/other/camera_controller.h"
#include "engine/scene/renderables.h"
#include "engine/scene/scene_list.h"

using namespace resmanager::literals;

namespace sm {
    static constexpr int SHADOW_MAP_UNIT = 2;
    static constexpr glm::vec3 CLEAR_COLOR = { 0.1f, 0.1f, 0.1f };
    static constexpr int BOUNDING_BOX_DIVISOR = 4;

    Renderer::Renderer(Ctx* ctx)
        : ctx(ctx) {
        render_helpers::clear_color(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b);
        render_helpers::enable_depth_test();
        render_helpers::initialize_stencil();

        initialize_uniform_buffers();
        initialize_skybox_renderer();
        initialize_screen_quad_renderer();
        initialize_quad3d_renderer();
        initialize_shadow_renderer();
        initialize_outline_renderer();
        initialize_bounding_box_renderer();
        initialize_origin_renderer();
        initialize_framebuffers();
        initialize_pixel_buffers();

        framebuffer_reader = FramebufferReader<4> {storage.pixel_buffers, storage.bounding_box_framebuffer};

        ctx->evt.connect<WindowResizedEvent, &Renderer::on_window_resized>(this);

        LOG_INFO("Initialized renderer");
    }

    Renderer::~Renderer() {
        LOG_INFO("Uninitialized renderer");
    }

    void Renderer::render(const SceneList& scene) {
        cache_camera_data();
        setup_uniform_buffers();
        setup_shadows();

        storage.shadow_map_framebuffer->bind();

        render_helpers::clear(render_helpers::Depth);
        render_helpers::viewport(storage.shadow_map_framebuffer->get_specification());

        // Render objects with shadows to depth buffer
        draw_models_to_depth_buffer(scene);

        storage.scene_framebuffer->bind();

        render_helpers::clear(render_helpers::Color | render_helpers::Depth | render_helpers::Stencil);
        render_helpers::viewport(storage.scene_framebuffer->get_specification());

        // Bind shadow map for use in shadow rendering
        render_helpers::bind_texture_2d(storage.shadow_map_framebuffer->get_depth_attachment(), SHADOW_MAP_UNIT);

        // Set to zero, because we are also rendering objects with outline later
        render_helpers::stencil_mask(0x00);

        // Render all normal models
        draw_models(scene);

        // Render all models with outline
        draw_models_with_outline(scene);

    #ifndef SM_BUILD_DISTRIBUTION
        if (origin) {
            draw_origin();
        }
    #endif

        // Render the skybox last (but before quads)
        if (storage.skybox_texture != nullptr) {
            draw_skybox();
        }

        // Render 3D quads
        draw_quads(scene);

        // Blit the resulted scene texture to an intermediate texture, resolving anti-aliasing
        storage.scene_framebuffer->blit(
            storage.intermediate_framebuffer.get(), ctx->properties->width, ctx->properties->height
        );

        storage.bounding_box_framebuffer->bind();

        render_helpers::clear(render_helpers::Color | render_helpers::Depth);
        render_helpers::viewport(storage.bounding_box_framebuffer->get_specification());

        // Render bounding boxes for models that are pickable
        draw_bounding_boxes(scene);

        // Read the texture for mouse picking
        const auto [mouse_x, mouse_y] = ctx->input.get_mouse();
        const int x = static_cast<int>(mouse_x) / BOUNDING_BOX_DIVISOR;
        const int y = (ctx->properties->height - static_cast<int>(mouse_y)) / BOUNDING_BOX_DIVISOR;
        framebuffer_reader.read(0, x, y);

        storage.intermediate_framebuffer->bind();

        render_helpers::viewport(storage.intermediate_framebuffer->get_specification());

        // Do post processing and render the final image to the screen
        end_rendering();

        float* data;
        framebuffer_reader.get<float>(&data);
        hovered_id = *data;

        validate_hovered_id(mouse_x, mouse_y);
    }

    void Renderer::add_post_processing(std::unique_ptr<PostProcessingStep>&& post_processing_step) {
        post_processing_step->prepare(post_processing_context);
        post_processing_context.steps.push_back(std::move(post_processing_step));
    }

    void Renderer::set_scene_framebuffer(int samples) {
        ASSERT(samples > 0, "Samples must be greater than 0");

        gl::FramebufferSpecification specification;
        specification.width = ctx->properties->width;
        specification.height = ctx->properties->height;
        specification.samples = samples;
        specification.color_attachments = {
            gl::Attachment {gl::AttachmentFormat::Rgba8, gl::AttachmentType::Renderbuffer}
        };
        specification.depth_attachment = gl::Attachment {
            gl::AttachmentFormat::Depth24Stencil8, gl::AttachmentType::Renderbuffer
        };

        storage.scene_framebuffer = std::make_shared<GlFramebuffer>(specification);

        ctx->purge_framebuffers();
        ctx->add_framebuffer(storage.scene_framebuffer);
    }

    void Renderer::set_shadow_map_framebuffer(int size) {
        ASSERT(size > 0, "Shadow map size must be greater than 0");

        shadow_map_size = size;

        gl::FramebufferSpecification specification;
        specification.width = shadow_map_size;
        specification.height = shadow_map_size;
        specification.depth_attachment = gl::Attachment {gl::AttachmentFormat::Depth32, gl::AttachmentType::Texture};
        specification.white_border_for_depth_texture = true;
        specification.resizable = false;

        storage.shadow_map_framebuffer = std::make_shared<GlFramebuffer>(specification);

        ctx->purge_framebuffers();
        ctx->add_framebuffer(storage.shadow_map_framebuffer);
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

        render_helpers::bind_texture_2d(texture, 0);

        render_helpers::draw_arrays(6);
    }

    void Renderer::post_processing() {
        post_processing_context.original_texture = storage.intermediate_framebuffer->get_color_attachment(0);
        post_processing_context.last_texture = post_processing_context.original_texture;
        post_processing_context.textures.clear();

        for (size_t i = 0; i < post_processing_context.steps.size(); i++) {
            const PostProcessingStep* step = post_processing_context.steps[i].get();

            step->framebuffer->bind();

            render_helpers::clear(render_helpers::Color);
            render_helpers::viewport(step->framebuffer->get_specification());

            step->render(post_processing_context);

            render_helpers::viewport(ctx->properties->width, ctx->properties->height);

            post_processing_context.last_texture = step->framebuffer->get_color_attachment(0);
            post_processing_context.textures.push_back(post_processing_context.last_texture);
        }
    }

    void Renderer::end_rendering() {
        storage.screen_quad_vertex_array->bind();

        render_helpers::disable_depth_test();
        render_helpers::clear_color(0.0f, 0.0f, 0.0f);

        post_processing();

        // Draw the final result to the screen; don't need clearing
        GlFramebuffer::bind_default();

        draw_screen_quad(post_processing_context.last_texture);

        render_helpers::clear_color(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b);
        render_helpers::enable_depth_test();

        gl::VertexArray::unbind();
    }

    void Renderer::draw_origin() {
        storage.origin_shader->bind();
        storage.origin_shader->upload_uniform_mat4("u_projection_view_matrix"_H, camera_cache.projection_view_matrix);

        storage.origin_vertex_array->bind();

        render_helpers::draw_arrays_lines(6);

        gl::VertexArray::unbind();
    }

    void Renderer::draw_skybox() {
        const glm::mat4& projection = camera_cache.projection_matrix;
        const glm::mat4 view = glm::mat4(glm::mat3(camera_cache.view_matrix));

        storage.skybox_shader->bind();
        storage.skybox_shader->upload_uniform_mat4("u_projection_view_matrix"_H, projection * view);

        storage.skybox_vertex_array->bind();
        storage.skybox_texture->bind(0);

        render_helpers::draw_arrays(36);

        gl::VertexArray::unbind();
    }

    void Renderer::draw_model(const renderables::Model* model) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale));

        model->vertex_array->bind();
        model->material->bind();

        model->material->get_shader()->upload_uniform_mat4("u_model_matrix"_H, matrix);

        render_helpers::draw_elements(model->index_buffer->get_index_count());
    }

    void Renderer::draw_model_with_outline(const renderables::Model* model) {
        render_helpers::stencil_mask(0xFF);

        draw_model(model);

        render_helpers::stencil_function(render_helpers::NotEqual, 1, 0xFF);
        render_helpers::stencil_mask(0x00);

        {
            static constexpr float SIZE = 3.6f;

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, model->position);
            matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(model->scale + SIZE));

            storage.outline_shader->bind();
            storage.outline_shader->upload_uniform_mat4("u_model_matrix"_H, matrix);
            storage.outline_shader->upload_uniform_vec3("u_color"_H, model->outline_color.value());  // Should never throw

            render_helpers::draw_elements(model->index_buffer->get_index_count());

            // Vertex array was bound in draw_model()
            gl::VertexArray::unbind();
        }

        render_helpers::stencil_function(render_helpers::Always, 1, 0xFF);
        render_helpers::stencil_mask(0xFF);
    }

    void Renderer::draw_models(const SceneList& scene) {
        for (size_t i = 0; i < scene.models.size(); i++) {
            const renderables::Model* model = scene.models[i];

            if (model->outline_color.has_value()) {
                continue;  // This model is rendered differently
            }

            draw_model(model);
        }

        // Don't unbind for every model
        gl::VertexArray::unbind();
    }

    void Renderer::draw_models_with_outline(const SceneList& scene) {
        static std::vector<const renderables::Model*> outline_models;

        outline_models.clear();

        std::for_each(scene.models.cbegin(), scene.models.cend(), [](const renderables::Model* model) {
            if (model->outline_color.has_value()) {
                outline_models.push_back(model);
            }
        });

        std::sort(outline_models.begin(), outline_models.end(), [this](const renderables::Model* lhs, const renderables::Model* rhs) {
            const float distance1 = glm::distance(lhs->position, camera_cache.position);
            const float distance2 = glm::distance(rhs->position, camera_cache.position);

            return distance1 < distance2;
        });

        for (const renderables::Model* model : outline_models) {
            draw_model_with_outline(model);
        }
    }

    void Renderer::draw_models_to_depth_buffer(const SceneList& scene) {
        storage.shadow_shader->bind();

        for (size_t i = 0; i < scene.models.size(); i++) {
            const renderables::Model* model = scene.models[i];

            if (model->cast_shadow) {
                glm::mat4 matrix = glm::mat4(1.0f);
                matrix = glm::translate(matrix, model->position);
                matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
                matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
                matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
                matrix = glm::scale(matrix, glm::vec3(model->scale));

                storage.shadow_shader->upload_uniform_mat4("u_model_matrix"_H, matrix);

                model->vertex_array->bind();

                render_helpers::draw_elements(model->index_buffer->get_index_count());
            }
        }

        // Don't unbind for every model
        gl::VertexArray::unbind();
    }

    void Renderer::draw_quad(const renderables::Quad* quad) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, quad->position);
        matrix = glm::scale(matrix, glm::vec3(quad->scale));

        storage.quad3d_shader->upload_uniform_mat4("u_model_matrix"_H, matrix);

        quad->texture->bind(0);

        render_helpers::draw_arrays(6);
    }

    void Renderer::draw_quads(const SceneList& scene) {
        storage.quad3d_vertex_array->bind();

        storage.quad3d_shader->bind();
        storage.quad3d_shader->upload_uniform_mat4("u_view_matrix"_H, camera_cache.view_matrix);

        static std::vector<const renderables::Quad*> quads;

        quads.clear();

        std::for_each(scene.quads.cbegin(), scene.quads.cend(), [&](const renderables::Quad* quad) {
            quads.push_back(quad);
        });

        std::sort(quads.begin(), quads.end(), [this](const renderables::Quad* lhs, const renderables::Quad* rhs) {
            const float distance1 = glm::distance(lhs->position, camera_cache.position);
            const float distance2 = glm::distance(rhs->position, camera_cache.position);

            return distance1 > distance2;
        });

        for (size_t i = 0; i < quads.size(); i++) {
            const renderables::Quad* quad = quads[i];

            draw_quad(quad);
        }

        gl::VertexArray::unbind();
    }

    void Renderer::add_bounding_box(const renderables::Model* model, std::vector<IdMatrix>& buffer_ids_transforms) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, model->bounding_box->size);

        const IdMatrix element = { model->bounding_box->id, matrix };
        buffer_ids_transforms.push_back(element);
    }

    void Renderer::draw_bounding_boxes(const SceneList& scene) {
        storage.box_vertex_array->bind();
        storage.box_shader->bind();

        static std::vector<const renderables::Model*> bounding_box_models;
        static std::vector<const renderables::Model*> bounding_box_models_unsorted;
        static std::vector<IdMatrix> buffer_ids_transforms;

        bounding_box_models.clear();
        bounding_box_models_unsorted.clear();
        buffer_ids_transforms.clear();

        std::for_each(scene.models.cbegin(), scene.models.cend(), [](const renderables::Model* model) {
            if (model->bounding_box.has_value()) {
                if (model->bounding_box->sort) {
                    bounding_box_models.push_back(model);
                } else {
                    bounding_box_models_unsorted.push_back(model);
                }
            }
        });

        std::sort(bounding_box_models.begin(), bounding_box_models.end(), [this](const renderables::Model* lhs, const renderables::Model* rhs) {
            const float distance1 = glm::distance(lhs->position, camera_cache.position);
            const float distance2 = glm::distance(rhs->position, camera_cache.position);

            return distance1 > distance2;
        });

        for (const renderables::Model* model : bounding_box_models_unsorted) {
            add_bounding_box(model, buffer_ids_transforms);
        }

        for (const renderables::Model* model : bounding_box_models) {
            add_bounding_box(model, buffer_ids_transforms);
        }

        storage.box_ids_transforms_buffer->bind();
        storage.box_ids_transforms_buffer->upload_data(
            buffer_ids_transforms.data(),
            sizeof(IdMatrix) * buffer_ids_transforms.size()
        );

        GlVertexBuffer::unbind();

        // Disable blending, because this is a floating-point framebuffer
        render_helpers::disable_blending();

        render_helpers::draw_elements_instanced(
            storage.box_index_buffer->get_index_count(),
            bounding_box_models.size() + bounding_box_models_unsorted.size()
        );

        render_helpers::enable_blending();

        gl::VertexArray::unbind();
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
            directional_light.position / light_space.position_divisor,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        const glm::mat4 light_space_matrix = projection * view;

        // Should already be configured
        storage.light_space_uniform_buffer->set(&light_space_matrix, 0);
        storage.light_space_uniform_buffer->bind();
        storage.light_space_uniform_buffer->upload_sub_data();

        GlUniformBuffer::unbind();
    }

    void Renderer::setup_uniform_buffers() {
        // Should already be configured
        storage.projection_view_uniform_buffer->set(&camera_cache.projection_view_matrix, 0);
        storage.projection_view_uniform_buffer->bind();
        storage.projection_view_uniform_buffer->upload_sub_data();

        if (storage.light_uniform_buffer->is_configured()) {
            storage.light_uniform_buffer->set(&directional_light.ambient_color, 0);
            storage.light_uniform_buffer->set(&directional_light.diffuse_color, 1);
            storage.light_uniform_buffer->set(&directional_light.specular_color, 2);
            storage.light_uniform_buffer->bind();
            storage.light_uniform_buffer->upload_sub_data();
        }

        if (storage.light_view_uniform_buffer->is_configured()) {
            storage.light_view_uniform_buffer->set(&directional_light.position, 0);
            storage.light_view_uniform_buffer->set(&camera_cache.position, 1);
            storage.light_view_uniform_buffer->bind();
            storage.light_view_uniform_buffer->upload_sub_data();
        }

        GlUniformBuffer::unbind();
    }

    void Renderer::validate_hovered_id(int x, int y) {
        if (x > ctx->properties->width || x < 0 || y > ctx->properties->height || y < 0) {
            hovered_id = Identifier::null;
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

    void Renderer::on_window_resized(const WindowResizedEvent&) {
        storage.quad3d_shader->bind();
        storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix"_H, camera_cache.projection_matrix);

        gl::Shader::unbind();
    }

    void Renderer::initialize_uniform_buffers() {
        storage.projection_view_uniform_buffer = std::make_shared<GlUniformBuffer>();
        storage.light_uniform_buffer = std::make_shared<GlUniformBuffer>();
        storage.light_view_uniform_buffer = std::make_shared<GlUniformBuffer>();
        storage.light_space_uniform_buffer = std::make_shared<GlUniformBuffer>();

        storage.projection_view_uniform_block.block_name = "ProjectionView";
        storage.projection_view_uniform_block.field_names = { "u_projection_view_matrix" };
        storage.projection_view_uniform_block.uniform_buffer = storage.projection_view_uniform_buffer;
        storage.projection_view_uniform_block.binding_index = 0;

        storage.light_uniform_block.block_name = "Light";
        storage.light_uniform_block.field_names = {
            "u_light_ambient",
            "u_light_diffuse",
            "u_light_specular"
        };
        storage.light_uniform_block.uniform_buffer = storage.light_uniform_buffer;
        storage.light_uniform_block.binding_index = 1;

        storage.light_view_uniform_block.block_name = "LightView";
        storage.light_view_uniform_block.field_names = {
            "u_light_position",
            "u_view_position"
        };
        storage.light_view_uniform_block.uniform_buffer = storage.light_view_uniform_buffer;
        storage.light_view_uniform_block.binding_index = 2;

        storage.light_space_uniform_block.block_name = "LightSpace";
        storage.light_space_uniform_block.field_names = { "u_light_space_matrix" };
        storage.light_space_uniform_block.uniform_buffer = storage.light_space_uniform_buffer;
        storage.light_space_uniform_block.binding_index = 3;
    }

    void Renderer::initialize_skybox_renderer() {
        storage.skybox_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(SKYBOX_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(SKYBOX_FRAGMENT_SHADER)),
            std::vector<std::string> {
                "u_projection_view_matrix",
                "u_skybox"
            }
        );

        storage.skybox_buffer = std::make_shared<GlVertexBuffer>(CUBEMAP_VERTICES, sizeof(CUBEMAP_VERTICES));

        VertexBufferLayout layout = VertexBufferLayout {}
            .add(0, VertexBufferLayout::Float, 3);

        storage.skybox_vertex_array = std::make_shared<gl::VertexArray>();
        storage.skybox_vertex_array->begin_definition()
            .add_buffer(storage.skybox_buffer, layout)
            .end_definition();
    }

    void Renderer::initialize_screen_quad_renderer() {
        storage.screen_quad_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(SCREEN_QUAD_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(SCREEN_QUAD_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );

        const float screen_quad_vertices[] = {
            -1.0f,  1.0f,
            -1.0f, -1.0f,
            1.0f,  1.0f,
            1.0f,  1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f,
        };

        storage.screen_quad_buffer = std::make_shared<GlVertexBuffer>(screen_quad_vertices, sizeof(screen_quad_vertices));

        VertexBufferLayout layout = VertexBufferLayout {}
            .add(0, VertexBufferLayout::Float, 2);

        storage.screen_quad_vertex_array = std::make_shared<gl::VertexArray>();
        storage.screen_quad_vertex_array->begin_definition()
            .add_buffer(storage.screen_quad_buffer, layout)
            .end_definition();
    }

    void Renderer::initialize_quad3d_renderer() {
        storage.quad3d_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(QUAD3D_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(QUAD3D_FRAGMENT_SHADER)),
            std::vector<std::string> {
                "u_model_matrix",
                "u_view_matrix",
                "u_projection_matrix",
                "u_texture"
            }
        );

        storage.quad3d_shader->bind();
        storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix"_H, camera_cache.projection_matrix);

        gl::Shader::unbind();

        const float quad3d_vertices[] = {
            -1.0f,  1.0f,    0.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
            1.0f,  1.0f,    1.0f, 1.0f,
            1.0f,  1.0f,    1.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
            1.0f, -1.0f,    1.0f, 0.0f
        };

        storage.quad3d_buffer = std::make_shared<GlVertexBuffer>(quad3d_vertices, sizeof(quad3d_vertices));

        VertexBufferLayout layout = VertexBufferLayout {}
            .add(0, VertexBufferLayout::Float, 2)
            .add(1, VertexBufferLayout::Float, 2);

        storage.quad3d_vertex_array = std::make_shared<gl::VertexArray>();
        storage.quad3d_vertex_array->begin_definition()
            .add_buffer(storage.quad3d_buffer, layout)
            .end_definition();
    }

    void Renderer::initialize_shadow_renderer() {
        storage.shadow_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(SHADOW_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(SHADOW_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_model_matrix" },
            std::initializer_list { storage.light_space_uniform_block }
        );
    }

    void Renderer::initialize_outline_renderer() {
        storage.outline_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(OUTLINE_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(OUTLINE_FRAGMENT_SHADER)),
            std::vector<std::string> {
                "u_model_matrix",
                "u_color"
            },
            std::initializer_list { storage.projection_view_uniform_block }
        );
    }

    void Renderer::initialize_bounding_box_renderer() {
        storage.box_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(BOUNDING_BOX_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(BOUNDING_BOX_FRAGMENT_SHADER)),
            std::vector<std::string> {},
            std::initializer_list { storage.projection_view_uniform_block }
        );

        const float box_vertices[] = {
            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f
        };

        const unsigned int box_indices[] = {
            0, 1, 2, 0, 2, 3,
            1, 5, 6, 1, 6, 2,
            5, 4, 6, 4, 7, 6,
            4, 0, 7, 0, 3, 7,
            3, 2, 6, 3, 6, 7,
            4, 5, 1, 4, 1, 0
        };

        storage.box_buffer = std::make_shared<GlVertexBuffer>(box_vertices, sizeof(box_vertices));
        storage.box_ids_transforms_buffer = std::make_shared<GlVertexBuffer>(gl::DrawHint::Stream);
        storage.box_index_buffer = std::make_shared<GlIndexBuffer>(box_indices, sizeof(box_indices));

        VertexBufferLayout layout = VertexBufferLayout {}
            .add(0, VertexBufferLayout::Float, 3);

        VertexBufferLayout layout_ids_transforms = VertexBufferLayout {}
            .add(1, VertexBufferLayout::Float, 1, true)
            .add(2, VertexBufferLayout::Float, 4, true)
            .add(3, VertexBufferLayout::Float, 4, true)
            .add(4, VertexBufferLayout::Float, 4, true)
            .add(5, VertexBufferLayout::Float, 4, true);

        storage.box_vertex_array = std::make_shared<gl::VertexArray>();
        storage.box_vertex_array->begin_definition()
            .add_buffer(storage.box_buffer, layout)
            .add_buffer(storage.box_ids_transforms_buffer, layout_ids_transforms)
            .add_index_buffer(storage.box_index_buffer)
            .end_definition();
    }

    void Renderer::initialize_origin_renderer() {
#ifdef SM_BUILD_DEBUG
            storage.origin_shader = std::make_shared<gl::Shader>(
                file_system::path_for_assets(ORIGIN_VERTEX_SHADER),
                file_system::path_for_assets(ORIGIN_FRAGMENT_SHADER),
                std::vector<std::string> { "u_projection_view_matrix" }
            );

        const float origin_vertices[] = {
            -20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
             20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
             0.0f,  -20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
             0.0f,   20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
             0.0f,    0.0f, -20.0f,    0.0f, 0.0f, 1.0f,
             0.0f,    0.0f,  20.0f,    0.0f, 0.0f, 1.0f
        };

        storage.origin_buffer = std::make_shared<GlVertexBuffer>(origin_vertices, sizeof(origin_vertices));

        VertexBufferLayout layout = VertexBufferLayout {}
            .add(0, VertexBufferLayout::Float, 3)
            .add(1, VertexBufferLayout::Float, 3);

        storage.origin_vertex_array = std::make_shared<gl::VertexArray>();
        storage.origin_vertex_array->begin_definition()
            .add_buffer(storage.origin_buffer, layout)
            .end_definition();
#endif
    }

    void Renderer::initialize_framebuffers() {
        {
            gl::FramebufferSpecification specification;
            specification.width = ctx->properties->width;
            specification.height = ctx->properties->height;
            specification.color_attachments = {
                gl::Attachment {gl::AttachmentFormat::Rgba8, gl::AttachmentType::Texture}
            };

            storage.intermediate_framebuffer = std::make_shared<GlFramebuffer>(specification);

            ctx->purge_framebuffers();
            ctx->add_framebuffer(storage.intermediate_framebuffer);
        }

        {
            gl::FramebufferSpecification specification;
            specification.width = ctx->properties->width / BOUNDING_BOX_DIVISOR;
            specification.height = ctx->properties->height / BOUNDING_BOX_DIVISOR;
            specification.resize_divisor = BOUNDING_BOX_DIVISOR;
            specification.color_attachments = {
                gl::Attachment {gl::AttachmentFormat::RedFloat, gl::AttachmentType::Renderbuffer}
            };
            specification.depth_attachment = gl::Attachment {
                gl::AttachmentFormat::Depth32, gl::AttachmentType::Renderbuffer
            };
            float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };  // TODO right now not used
            specification.clear_drawbuffer = 0;
            memcpy(specification.color_clear_value, color, sizeof(float) * 4);

            storage.bounding_box_framebuffer = std::make_shared<GlFramebuffer>(specification);

            ctx->purge_framebuffers();
            ctx->add_framebuffer(storage.bounding_box_framebuffer);
        }
    }

    void Renderer::initialize_pixel_buffers() {
        storage.pixel_buffers = {
            std::make_shared<GlPixelBuffer>(sizeof(float)),
            std::make_shared<GlPixelBuffer>(sizeof(float)),
            std::make_shared<GlPixelBuffer>(sizeof(float)),
            std::make_shared<GlPixelBuffer>(sizeof(float))
        };
    }
}
