#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_truetype.h>

#include "application/application.h"
#include "application/platform.h"
#include "application/events.h"
#include "application/input.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/framebuffer_reader.h"
#include "graphics/renderer/opengl/vertex_array.h"
#include "graphics/renderer/opengl/buffer.h"
#include "graphics/renderer/opengl/shader.h"
#include "graphics/renderer/opengl/texture.h"
#include "graphics/renderer/opengl/framebuffer.h"
#include "other/paths.h"
#include "other/logging.h"
#include "other/assert.h"
#include "other/encryption.h"

#define ERASE(vector, search_handle) \
    { \
        auto iter = std::find_if(vector.begin(), vector.end(), [search_handle](const Model* quad) { \
            return quad->handle == (search_handle); \
        }); \
        if (iter != vector.end()) { \
            vector.erase(iter); \
        } \
    }

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

    storage.projection_view_uniform_buffer = UniformBuffer::create();
    storage.light_uniform_buffer = UniformBuffer::create();
    storage.light_view_position_uniform_buffer = UniformBuffer::create();
    storage.light_space_uniform_buffer = UniformBuffer::create();

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

    maybe_initialize_assets();

    using namespace encryption;

    {
        std::vector<std::string> uniforms = {
            "u_projection_view_matrix",
            "u_skybox"
        };
        storage.skybox_shader = Shader::create(
            encr(paths::path_for_assets(SKYBOX_VERTEX_SHADER)),
            encr(paths::path_for_assets(SKYBOX_FRAGMENT_SHADER)),
            uniforms
        );
    }

    {
        std::vector<std::string> uniforms = {
            "u_screen_texture"
        };
        storage.screen_quad_shader = Shader::create(
            encr(paths::path_for_assets(SCREEN_QUAD_VERTEX_SHADER)),
            encr(paths::path_for_assets(SCREEN_QUAD_FRAGMENT_SHADER)),
            uniforms
        );
    }

    {
        std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_view_matrix",
            "u_projection_matrix",
            "u_texture"
        };
        storage.quad3d_shader = Shader::create(
            encr(paths::path_for_assets(QUAD3D_VERTEX_SHADER)),
            encr(paths::path_for_assets(QUAD3D_FRAGMENT_SHADER)),
            uniforms
        );
    }

    {
        std::vector<std::string> uniforms = {
            "u_model_matrix"
        };
        storage.shadow_shader = Shader::create(
            encr(paths::path_for_assets(SHADOW_VERTEX_SHADER)),
            encr(paths::path_for_assets(SHADOW_FRAGMENT_SHADER)),
            uniforms,
            { storage.light_space_uniform_block }
        );
    }

    {
        std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_color"
        };
        storage.outline_shader = Shader::create(
            encr(paths::path_for_assets(OUTLINE_VERTEX_SHADER)),
            encr(paths::path_for_assets(OUTLINE_FRAGMENT_SHADER)),
            uniforms,
            { storage.projection_view_uniform_block }
        );
    }

#ifdef PLATFORM_GAME_DEBUG
    {
        std::vector<std::string> uniforms = {
            "u_projection_view_matrix"
        };
        storage.origin_shader = Shader::create(
            paths::path_for_assets(ORIGIN_VERTEX_SHADER),
            paths::path_for_assets(ORIGIN_FRAGMENT_SHADER),
            uniforms
        );
    }
#endif

    {
        std::shared_ptr<Buffer> buffer = Buffer::create(SKYBOX_VERTICES, sizeof(SKYBOX_VERTICES));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        storage.skybox_vertex_array = VertexArray::create();
        storage.skybox_vertex_array->add_buffer(buffer, layout);

        VertexArray::unbind();
    }

    {
        float screen_quad_vertices[] = {
            -1.0f,  1.0f,    0.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
             1.0f,  1.0f,    1.0f, 1.0f,
             1.0f,  1.0f,    1.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
             1.0f, -1.0f,    1.0f, 0.0f
        };

        std::shared_ptr<Buffer> buffer = Buffer::create(screen_quad_vertices, sizeof(screen_quad_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 2);
        layout.add(1, BufferLayout::Type::Float, 2);
        storage.quad_vertex_array = VertexArray::create();
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
        std::shared_ptr<Buffer> buffer = Buffer::create(origin_vertices, sizeof(origin_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 3);
        layout.add(1, BufferLayout::Type::Float, 3);
        storage.origin_vertex_array = VertexArray::create();
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

        storage.intermediate_framebuffer = Framebuffer::create(specification);

        app->purge_framebuffers();
        app->add_framebuffer(storage.intermediate_framebuffer);
    }

    storage.pixel_buffers = {
        PixelBuffer::create(sizeof(int)),
        PixelBuffer::create(sizeof(int)),
        PixelBuffer::create(sizeof(int)),
        PixelBuffer::create(sizeof(int))
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
    const glm::mat4& projection_view_matrix = app->camera.get_projection_view_matrix();

    storage.projection_view_uniform_buffer->set(&projection_view_matrix, 0);
    storage.projection_view_uniform_buffer->bind();
    storage.projection_view_uniform_buffer->upload_data();

    storage.light_uniform_buffer->set(&light.ambient_color, 0);
    storage.light_uniform_buffer->set(&light.diffuse_color, 1);
    storage.light_uniform_buffer->set(&light.specular_color, 2);
    storage.light_uniform_buffer->bind();
    storage.light_uniform_buffer->upload_data();

    storage.light_view_position_uniform_buffer->set(&light.position, 0);
    storage.light_view_position_uniform_buffer->set(&app->camera.get_position(), 1);
    storage.light_view_position_uniform_buffer->bind();
    storage.light_view_position_uniform_buffer->upload_data();

    setup_shadows();
    storage.depth_map_framebuffer->bind();

    clear(Depth);
    set_viewport(shadow_map_size, shadow_map_size);

    // Render objects with shadows to depth buffer
    draw_models_to_depth_buffer();

    storage.scene_framebuffer->bind();

    clear(Color | Depth | Stencil);
    set_viewport(app->app_data.width, app->app_data.height);

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

    Framebuffer::bind_default();

    // Draw the result to the screen
    clear(Color);
    draw_screen_quad(storage.intermediate_framebuffer->get_color_attachment(0));

    int* data;
    reader.get<int>(&data);
    hovered_id = *data;

    check_hovered_id(x, y);
}

void Renderer::on_window_resized(events::WindowResizedEvent& event) {
    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix", app->camera.get_projection_matrix());
}

void Renderer::add_model(Model& model, int options) {
    static unsigned int handle = 0;

    const bool with_outline = options & static_cast<int>(WithOutline);
    const bool cast_shadow = options & static_cast<int>(CastShadow);

    model.handle = ++handle;

    if (!with_outline) {
        models.push_back(&model);
    } else {
        models_outline.push_back(&model);
    }

    if (cast_shadow) {
        models_cast_shadow.push_back(&model);
    }
}

void Renderer::remove_model(unsigned int handle) {
    ERASE(models, handle)
    ERASE(models_outline, handle)
    ERASE(models_cast_shadow, handle)
}

void Renderer::update_model(Model& model, int options) {
    remove_model(model.handle);

    const bool with_outline = options & static_cast<int>(WithOutline);
    const bool cast_shadow = options & static_cast<int>(CastShadow);

    if (!with_outline) {
        models.push_back(&model);
    } else {
        models_outline.push_back(&model);
    }

    if (cast_shadow) {
        models_cast_shadow.push_back(&model);
    }
}

void Renderer::add_quad(Quad& quad) {
    static unsigned int handle = 0;

    quad.handle = ++handle;

    quads.push_back(&quad);
}

void Renderer::remove_quad(unsigned int handle) {
    auto iter = std::find_if(quads.begin(), quads.end(), [handle](const Quad* quad) {
        return quad->handle == handle;
    });

    if (iter != quads.end()) {
        quads.erase(iter);
    }
}

void Renderer::clear() {
    models.clear();
    models_outline.clear();
    models_cast_shadow.clear();
    quads.clear();
}

void Renderer::set_viewport(int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::set_clear_color(float red, float green, float blue) {
    glClearColor(red, green, blue, 1.0f);
}

void Renderer::set_scene_framebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    storage.scene_framebuffer = framebuffer;

    app->purge_framebuffers();
    app->add_framebuffer(storage.scene_framebuffer);
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

    storage.depth_map_framebuffer = Framebuffer::create(specification);

    app->purge_framebuffers();
    app->add_framebuffer(storage.depth_map_framebuffer);
}

void Renderer::setup_shader(std::shared_ptr<Shader> shader) {
    const std::vector<std::string>& uniforms = shader->get_uniforms();

    if (std::find(uniforms.begin(), uniforms.end(), "u_shadow_map") != uniforms.end()) {
        shader->bind();
        shader->upload_uniform_int("u_shadow_map", SHADOW_MAP_UNIT);
        Shader::unbind();
    }
}

void Renderer::clear(int buffers) {
    glClear(buffers);
}

void Renderer::draw_screen_quad(GLuint texture) {
    storage.screen_quad_shader->bind();
    storage.quad_vertex_array->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

#ifdef PLATFORM_GAME_DEBUG
void Renderer::draw_origin() {
    storage.origin_shader->bind();
    storage.origin_shader->upload_uniform_mat4("u_projection_view_matrix", app->camera.get_projection_view_matrix());

    storage.origin_vertex_array->bind();

    glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDrawArrays(GL_LINES, 0, 6);
    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
#endif

void Renderer::draw_skybox() {
    const glm::mat4& projection = app->camera.get_projection_matrix();
    const glm::mat4 view = glm::mat4(glm::mat3(app->camera.get_view_matrix()));

    storage.skybox_shader->bind();
    storage.skybox_shader->upload_uniform_mat4("u_projection_view_matrix", projection * view);

    storage.skybox_vertex_array->bind();
    storage.skybox_texture->bind(0);

    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

void Renderer::draw_model(const Model* model) {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, model->position);
    matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

    model->vertex_array->bind();
    model->material->bind();

    model->material->get_shader()->upload_uniform_mat4("u_model_matrix", matrix);

    glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
}

void Renderer::draw_models_to_depth_buffer() {
    storage.shadow_shader->bind();

    for (const Model* model : models_cast_shadow) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

        storage.shadow_shader->upload_uniform_mat4("u_model_matrix", matrix);

        model->vertex_array->bind();

        glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
    }
}

void Renderer::draw_models_normal() {
    static std::vector<Model*> hoverable_models;
    static std::vector<Model*> non_hoverable_models;

    hoverable_models.clear();
    non_hoverable_models.clear();

    std::for_each(models.begin(), models.end(), [&](Model* model) {
        if (model->material->is_hoverable()) {
            hoverable_models.push_back(model);
        } else {
            non_hoverable_models.push_back(model);
        }
    });

    glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    for (const Model* model : non_hoverable_models) {
        draw_model(model);
    }

    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    for (const Model* model : hoverable_models) {
        draw_model(model);
    }
}

void Renderer::draw_models_with_outline() {
    static std::vector<Model*> hoverable_models;
    static std::vector<Model*> non_hoverable_models;

    hoverable_models.clear();
    non_hoverable_models.clear();

    std::sort(models_outline.begin(), models_outline.end(), [this](const Model* left, const Model* right) {
        const float distance1 = glm::distance(left->position, app->camera.get_position());
        const float distance2 = glm::distance(right->position, app->camera.get_position());

        return distance1 < distance2;
    });

    std::for_each(models_outline.begin(), models_outline.end(), [&](Model* model) {
        if (model->material->is_hoverable()) {
            hoverable_models.push_back(model);
        } else {
            non_hoverable_models.push_back(model);
        }
    });

    glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    _draw_models_with_outline(non_hoverable_models);

    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    _draw_models_with_outline(hoverable_models);
}

void Renderer::_draw_models_with_outline(const std::vector<Model*>& submodels) {
    for (const Model* model : submodels) {
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
            matrix = glm::scale(matrix, glm::vec3(model->scale + SIZE, model->scale + SIZE, model->scale + SIZE));

            storage.outline_shader->bind();
            storage.outline_shader->upload_uniform_mat4("u_model_matrix", matrix);
            storage.outline_shader->upload_uniform_vec3("u_color", model->outline_color);

            // Render without output to red
            glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
            glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }
}

void Renderer::draw_quads() {
    storage.quad3d_shader->bind();
    storage.quad3d_shader->upload_uniform_mat4("u_view_matrix", app->camera.get_view_matrix());
    storage.quad3d_shader->upload_uniform_mat4("u_projection_matrix", app->camera.get_projection_matrix());

    storage.quad_vertex_array->bind();

    std::sort(quads.begin(), quads.end(), [this](const Quad* left, const Quad* right) {
        const float distance1 = glm::distance(left->position, app->camera.get_position());
        const float distance2 = glm::distance(right->position, app->camera.get_position());

        return distance1 > distance2;
    });

    for (const Quad* quad : quads) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, quad->position);
        matrix = glm::scale(matrix, glm::vec3(quad->scale, quad->scale, quad->scale));

        storage.quad3d_shader->upload_uniform_mat4("u_model_matrix", matrix);

        quad->texture->bind(0);

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

void Renderer::check_hovered_id(int x, int y) {
    if (x > app->app_data.width || x < 0 || y > app->app_data.height || y < 0) {
        hovered_id = hoverable::null;
    }
}

void Renderer::maybe_initialize_assets() {
#ifdef PLATFORM_GAME_RELEASE
    static const char* PREFIX = ".dat";

    static const std::array<std::string*, 10> assets = { 
        &SHADOW_VERTEX_SHADER,
        &SHADOW_FRAGMENT_SHADER,
        &SCREEN_QUAD_VERTEX_SHADER,
        &SCREEN_QUAD_FRAGMENT_SHADER,
        &OUTLINE_VERTEX_SHADER,
        &OUTLINE_FRAGMENT_SHADER,
        &SKYBOX_VERTEX_SHADER,
        &SKYBOX_FRAGMENT_SHADER,
        &QUAD3D_VERTEX_SHADER,
        &QUAD3D_FRAGMENT_SHADER
    };

    for (std::string* asset : assets) {
        *asset += PREFIX;
    }
#endif
}
