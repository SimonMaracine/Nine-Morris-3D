#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_truetype.h>

#include "nine_morris_3d_engine/application/application.h"
#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/application/input.h"
#include "nine_morris_3d_engine/ecs/internal_components.h"
#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/framebuffer_reader.h"
#include "nine_morris_3d_engine/graphics/renderer/post_processing_step.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/texture.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/framebuffer.h"
#include "nine_morris_3d_engine/other/paths.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encrypt.h"

constexpr int SHADOW_MAP_UNIT = 2;

Renderer::Renderer(Application* app)
    : app(app) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    storage.projection_view_uniform_buffer = std::make_shared<UniformBuffer>();
    storage.light_uniform_buffer = std::make_shared<UniformBuffer>();
    storage.light_view_position_uniform_buffer = std::make_shared<UniformBuffer>();
    storage.light_space_uniform_buffer = std::make_shared<UniformBuffer>();

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
        storage.skybox_shader = std::make_shared<Shader>(
            encr(paths::path_for_assets(SKYBOX_VERTEX_SHADER)),
            encr(paths::path_for_assets(SKYBOX_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_projection_view_matrix", "u_skybox" }
        );
    }

    {
        storage.screen_quad_shader = std::make_shared<Shader>(
            encr(paths::path_for_assets(SCREEN_QUAD_VERTEX_SHADER)),
            encr(paths::path_for_assets(SCREEN_QUAD_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_screen_texture" }
        );
    }

    {
        storage.quad3d_shader = std::make_shared<Shader>(
            encr(paths::path_for_assets(QUAD3D_VERTEX_SHADER)),
            encr(paths::path_for_assets(QUAD3D_FRAGMENT_SHADER)),
            std::vector<std::string> {
                "u_model_matrix",
                "u_view_matrix",
                "u_projection_matrix",
                "u_texture"
            }
        );
    }

    {
        storage.shadow_shader = std::make_shared<Shader>(
            encr(paths::path_for_assets(SHADOW_VERTEX_SHADER)),
            encr(paths::path_for_assets(SHADOW_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_model_matrix" },
            std::vector { storage.light_space_uniform_block }
        );
    }

    {
        storage.outline_shader = std::make_shared<Shader>(
            encr(paths::path_for_assets(OUTLINE_VERTEX_SHADER)),
            encr(paths::path_for_assets(OUTLINE_FRAGMENT_SHADER)),
            std::vector<std::string> { "u_model_matrix", "u_color" },
            std::vector { storage.projection_view_uniform_block }
        );
    }

#ifdef PLATFORM_GAME_DEBUG
    {
        storage.origin_shader = std::make_shared<Shader>(
            paths::path_for_assets(ORIGIN_VERTEX_SHADER),
            paths::path_for_assets(ORIGIN_FRAGMENT_SHADER),
            std::vector<std::string> { "u_projection_view_matrix" }
        );
    }
#endif

    {
        std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(SKYBOX_VERTICES, sizeof(SKYBOX_VERTICES));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        storage.skybox_vertex_array = std::make_shared<VertexArray>();
        storage.skybox_vertex_array->add_buffer(buffer, layout);

        VertexArray::unbind();
    }

    {
        float screen_quad_vertices[] = {
            -1.0f,  1.0f,
            -1.0f, -1.0f,
             1.0f,  1.0f,
             1.0f,  1.0f,
            -1.0f, -1.0f,
             1.0f, -1.0f,
        };

        std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(screen_quad_vertices, sizeof(screen_quad_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 2);
        storage.quad_vertex_array = std::make_shared<VertexArray>();
        storage.quad_vertex_array->add_buffer(buffer, layout);

        VertexArray::unbind();
    }

#ifdef PLATFORM_GAME_DEBUG
    {
        float origin_vertices[] = {
            -20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
             20.0f,   0.0f,   0.0f,    1.0f, 0.0f, 0.0f,
              0.0f, -20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
              0.0f,  20.0f,   0.0f,    0.0f, 1.0f, 0.0f,
              0.0f,   0.0f, -20.0f,    0.0f, 0.0f, 1.0f,
              0.0f,   0.0f,  20.0f,    0.0f, 0.0f, 1.0f
        };
        std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(origin_vertices, sizeof(origin_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        layout.add(1, BufferLayout::Type::Float, 3);
        storage.origin_vertex_array = std::make_shared<VertexArray>();
        storage.origin_vertex_array->add_buffer(buffer, layout);

        VertexArray::unbind();
    }
#endif

    {
        FramebufferSpecification specification;
        specification.width = app->app_data.width;
        specification.height = app->app_data.height;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture),
            Attachment(AttachmentFormat::RED_I, AttachmentType::Texture)
        };
        specification.depth_attachment = Attachment(
            AttachmentFormat::DEPTH24_STENCIL8, AttachmentType::Renderbuffer
        );

        storage.intermediate_framebuffer = std::make_shared<Framebuffer>(specification);

        purge_framebuffers();
        add_framebuffer(storage.intermediate_framebuffer);
    }

    storage.pixel_buffers = {
        std::make_shared<PixelBuffer>(sizeof(int)),
        std::make_shared<PixelBuffer>(sizeof(int)),
        std::make_shared<PixelBuffer>(sizeof(int)),
        std::make_shared<PixelBuffer>(sizeof(int))
    };

    reader = FramebufferReader<4>(storage.pixel_buffers, storage.intermediate_framebuffer);

    // Setup uniform variables
    storage.screen_quad_shader->bind();
    storage.screen_quad_shader->upload_uniform_int("u_screen_texture", 0);

    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_int("u_texture", 0);

    Shader::unbind();

    DEB_INFO("Initialized renderer");
}

Renderer::~Renderer() {
    DEB_INFO("Destroyed renderer");
}

void Renderer::render() {
    cache_camera();
    setup_uniform_buffers();

    setup_shadows();
    storage.depth_map_framebuffer->bind();

    render_helpers::clear(render_helpers::Depth);
    render_helpers::viewport(shadow_map_size, shadow_map_size);

    // Sort entities that have a transform
    app->registry.sort<TransformComponent>([this](const TransformComponent& lhs, const TransformComponent& rhs) {
        const float distance1 = glm::distance(lhs.position, camera_cache.position);
        const float distance2 = glm::distance(rhs.position, camera_cache.position);

        return distance1 < distance2;
    });

    // Render objects with shadows to depth buffer
    draw_models_to_depth_buffer();

    storage.scene_framebuffer->bind();

    render_helpers::clear(render_helpers::Color | render_helpers::Depth | render_helpers::Stencil);
    render_helpers::viewport(app->app_data.width, app->app_data.height);

    // Bind shadow map for use in shadow rendering
    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_UNIT);
    glBindTexture(GL_TEXTURE_2D, storage.depth_map_framebuffer->get_depth_attachment());

    // Set to zero, because we are also rendering objects with outline later
    glStencilMask(0x00);

    if (storage.skybox_texture != nullptr) {
        draw_skybox();
    }

    // Render all normal models
    draw_models_normal();

    // Render all models with outline
    draw_models_with_outline();

#ifdef PLATFORM_GAME_DEBUG
    if (origin) {
        draw_origin();
    }
#endif

    // Render quads
    draw_quads();

    // Blit the scene texture result to an intermediate texture resolving anti-aliasing
    storage.scene_framebuffer->resolve_framebuffer(
        storage.intermediate_framebuffer->get_id(), app->app_data.width, app->app_data.height
    );

    storage.intermediate_framebuffer->bind();

    // Read the texture for mouse picking
    const int x = static_cast<int>(input::get_mouse_x());
    const int y = app->app_data.height - static_cast<int>(input::get_mouse_y());
    reader.read(1, x, y);

    // Do post processing and render the final image to the screen
    end_rendering();

    int* data;
    reader.get<int>(&data);
    hovered_id = *data;

    check_hovered_id(x, y);
}

void Renderer::on_window_resized(const WindowResizedEvent& event) {
    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix", camera_cache.projection_matrix);
}

void Renderer::set_scene_framebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    storage.scene_framebuffer = framebuffer;

    purge_framebuffers();
    add_framebuffer(storage.scene_framebuffer);
}

void Renderer::set_skybox(std::shared_ptr<Texture3D> texture) {
    storage.skybox_texture = texture;
}

void Renderer::set_depth_map_framebuffer(int size) {
    ASSERT(size > 0, "Shadow map size must be greater than 0");

    shadow_map_size = size;

    FramebufferSpecification specification;
    specification.width = shadow_map_size;
    specification.height = shadow_map_size;
    specification.depth_attachment = Attachment(AttachmentFormat::DEPTH32, AttachmentType::Texture);
    specification.white_border_for_depth_texture = true;
    specification.resizable = false;

    storage.depth_map_framebuffer = std::make_shared<Framebuffer>(specification);

    purge_framebuffers();
    add_framebuffer(storage.depth_map_framebuffer);
}

void Renderer::add_framebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    framebuffers.push_back(framebuffer);
}

void Renderer::purge_framebuffers() {
    std::vector<size_t> indices;

    for (size_t i = 0; i < framebuffers.size(); i++) {
        if (framebuffers[i].expired()) {
            indices.push_back(i);
        }
    }

    for (int64_t i = framebuffers.size() - 1; i >= 0; i--) {
        for (size_t index : indices) {
            if (static_cast<int64_t>(index) == i) {
                framebuffers.erase(std::next(framebuffers.begin(), index));
                break;
            }
        }
    }
}

void Renderer::setup_shader(std::shared_ptr<Shader> shader) {
    const std::vector<std::string>& uniforms = shader->get_uniforms();

    if (std::find(uniforms.begin(), uniforms.end(), "u_shadow_map") != uniforms.end()) {
        shader->bind();
        shader->upload_uniform_int("u_shadow_map", SHADOW_MAP_UNIT);
        Shader::unbind();
    }
}

void Renderer::add_post_processing(std::shared_ptr<PostProcessingStep> post_processing_step) {
    post_processing_context.steps.push_back(post_processing_step);
    post_processing_step->prepare(post_processing_context);
}

void Renderer::draw_screen_quad(GLuint texture) {
    storage.screen_quad_shader->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::post_processing() {
    post_processing_context.original_texture = storage.intermediate_framebuffer->get_color_attachment(0);
    post_processing_context.last_texture = storage.intermediate_framebuffer->get_color_attachment(0);
    post_processing_context.textures.clear();

    for (size_t i = 0; i < post_processing_context.steps.size(); i++) {
        const PostProcessingStep* step = post_processing_context.steps[i].get();

        if (!step->enabled) {
            continue;
        }

        const FramebufferSpecification& specification = step->framebuffer->get_specification();

        step->framebuffer->bind();
        render_helpers::clear(render_helpers::Color);
        render_helpers::viewport(specification.width, specification.height);
        step->render(post_processing_context);
        render_helpers::viewport(app->app_data.width, app->app_data.height);

        post_processing_context.last_texture = step->framebuffer->get_color_attachment(0);
        post_processing_context.textures.push_back(step->framebuffer->get_color_attachment(0));
    }
}

void Renderer::end_rendering() {
    storage.quad_vertex_array->bind();

    glDisable(GL_DEPTH_TEST);

    post_processing();

    // Draw the final result to the screen
    Framebuffer::bind_default();
    render_helpers::clear(render_helpers::Color);
    draw_screen_quad(post_processing_context.last_texture);

    glEnable(GL_DEPTH_TEST);
}

#ifdef PLATFORM_GAME_DEBUG
void Renderer::draw_origin() {
    storage.origin_shader->bind();
    storage.origin_shader->upload_uniform_mat4("u_projection_view_matrix", camera_cache.projection_view_matrix);

    storage.origin_vertex_array->bind();

    glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDrawArrays(GL_LINES, 0, 6);
    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
#endif

void Renderer::draw_skybox() {
    const glm::mat4& projection = camera_cache.projection_matrix;
    const glm::mat4 view = glm::mat4(glm::mat3(camera_cache.view_matrix));

    storage.skybox_shader->bind();
    storage.skybox_shader->upload_uniform_mat4("u_projection_view_matrix", projection * view);

    storage.skybox_vertex_array->bind();
    storage.skybox_texture->bind(0);

    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

void Renderer::draw_model(const ModelComponent& model_c, const TransformComponent& transform_c) {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, transform_c.position);
    matrix = glm::rotate(matrix, transform_c.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, transform_c.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, transform_c.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, transform_c.scale);

    model_c.vertex_array->bind();
    model_c.material->bind();

    model_c.material->get_shader()->upload_uniform_mat4("u_model_matrix", matrix);

    glDrawElements(GL_TRIANGLES, model_c.index_count, GL_UNSIGNED_INT, nullptr);
}

void Renderer::draw_model_with_outline(const ModelComponent& model_c, const TransformComponent& transform_c, const OutlineComponent& outline_c) {
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    draw_model(model_c, transform_c);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    {
        constexpr float SIZE = 3.6f;

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, transform_c.position);
        matrix = glm::rotate(matrix, transform_c.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, transform_c.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, transform_c.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, transform_c.scale + SIZE);

        storage.outline_shader->bind();
        storage.outline_shader->upload_uniform_mat4("u_model_matrix", matrix);
        storage.outline_shader->upload_uniform_vec3("u_color", outline_c.outline_color);  // FIXME use outline_enabled or not?

        // Render without output to red
        glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDrawElements(GL_TRIANGLES, model_c.index_count, GL_UNSIGNED_INT, nullptr);
        glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void Renderer::draw_models_to_depth_buffer() {
    storage.shadow_shader->bind();

    auto view = app->registry.view<RenderComponent, ModelComponent, TransformComponent, CastShadowComponent>();

    for (auto entity : view) {
        auto [model_c, transform_c] = view.get<ModelComponent, TransformComponent>(entity);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, transform_c.position);
        matrix = glm::rotate(matrix, transform_c.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, transform_c.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, transform_c.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, transform_c.scale);

        storage.shadow_shader->upload_uniform_mat4("u_model_matrix", matrix);

        model_c.vertex_array->bind();

        glDrawElements(GL_TRIANGLES, model_c.index_count, GL_UNSIGNED_INT, nullptr);
    }
}

void Renderer::draw_models_normal() {
    // Draw non-hoverable models
    {
        glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);

        auto view = app->registry.view<RenderComponent, ModelComponent, TransformComponent>();

        for (auto entity : view) {
            auto [model_c, transform_c] = view.get<ModelComponent, TransformComponent>(entity);

            draw_model(model_c, transform_c);
        }
    }

    // Draw hoverable models
    {
        glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        auto view = app->registry.view<RenderComponent, ModelComponent, TransformComponent, HoverComponent>();

        for (auto entity : view) {
            auto [model_c, transform_c] = view.get<ModelComponent, TransformComponent>(entity);

            draw_model(model_c, transform_c);
        }
    }
}

void Renderer::draw_models_with_outline() {
    // Draw non-hoverable models
    {
        glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);

        auto view = app->registry.view<RenderComponent, ModelComponent, TransformComponent, OutlineComponent>();
        view = view.use<TransformComponent>();  // TODO ???

        for (auto entity : view) {
            auto [model_c, transform_c, outline_c] = view.get<ModelComponent, TransformComponent, OutlineComponent>(entity);

            if (!outline_c.outline_enabled) {
                continue;
            }

            draw_model_with_outline(model_c, transform_c, outline_c);
        }
    }

    // Draw hoverable models
    {
        glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        auto view = app->registry.view<RenderComponent, ModelComponent, TransformComponent, HoverComponent, OutlineComponent>();
        view = view.use<TransformComponent>();  // TODO ???

        for (auto entity : view) {
            auto [model_c, transform_c, outline_c] = view.get<ModelComponent, TransformComponent, OutlineComponent>(entity);

            if (!outline_c.outline_enabled) {
                continue;
            }

            draw_model_with_outline(model_c, transform_c, outline_c);
        }
    }
}

void Renderer::draw_quads() {
    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_mat4("u_view_matrix", camera_cache.view_matrix);
    storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix", camera_cache.projection_matrix);

    storage.quad_vertex_array->bind();

    auto view = app->registry.view<RenderComponent, QuadComponent, TransformComponent>();

    for (auto entity : view) {
        auto [quad_c, transform_c] = view.get<QuadComponent, TransformComponent>(entity);

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, transform_c.position);
        matrix = glm::scale(matrix, transform_c.scale);

        storage.quad3d_shader->upload_uniform_mat4("u_model_matrix", matrix);

        quad_c.texture->bind(0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Renderer::setup_shadows() {
    const glm::mat4 projection = glm::ortho(
        light_space.left,
        light_space.right,
        light_space.bottom,
        light_space.top,
        light_space.near,
        light_space.far
    );
    const glm::mat4 view = glm::lookAt(
        light.position / light_space.light_divisor,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    const glm::mat4 light_space_matrix = projection * view;

    storage.light_space_uniform_buffer->set(&light_space_matrix, 0);
    storage.light_space_uniform_buffer->bind();
    storage.light_space_uniform_buffer->upload_data();
}

void Renderer::setup_uniform_buffers() {
    storage.projection_view_uniform_buffer->set(&camera_cache.projection_view_matrix, 0);
    storage.projection_view_uniform_buffer->bind();
    storage.projection_view_uniform_buffer->upload_data();

    storage.light_uniform_buffer->set(&light.ambient_color, 0);
    storage.light_uniform_buffer->set(&light.diffuse_color, 1);
    storage.light_uniform_buffer->set(&light.specular_color, 2);
    storage.light_uniform_buffer->bind();
    storage.light_uniform_buffer->upload_data();

    storage.light_view_position_uniform_buffer->set(&light.position, 0);
    storage.light_view_position_uniform_buffer->set(&camera_cache.position, 1);
    storage.light_view_position_uniform_buffer->bind();
    storage.light_view_position_uniform_buffer->upload_data();
}

void Renderer::check_hovered_id(int x, int y) {
    if (x > app->app_data.width || x < 0 || y > app->app_data.height || y < 0) {
        hovered_id = hover::null;
    }
}

void Renderer::cache_camera() {
    // Use the last camera in the registry
    app->registry.view<CameraComponent>().each([this](entt::entity, CameraComponent& camera_c) {
        camera_cache.projection_view_matrix = camera_c.camera.get_projection_view_matrix();
        camera_cache.view_matrix = camera_c.camera.get_view_matrix();
        camera_cache.projection_matrix = camera_c.camera.get_projection_matrix();
        camera_cache.position = camera_c.camera.get_position();
    });
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
