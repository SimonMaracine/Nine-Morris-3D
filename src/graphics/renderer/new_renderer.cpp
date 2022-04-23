#include <memory>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_truetype.h>

#include "application/application.h"
#include "application/platform.h"
#include "application/input.h"
#include "graphics/renderer/new_renderer.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/framebuffer.h"
#include "graphics/renderer/framebuffer_reader.h"
#include "other/logging.h"

const char* projection_view_block_fields[] = {
    "u_projection_view_matrix"
};

const char* light_block_fields[] = {
    "u_light_position",
    "u_light_ambient",
    "u_light_diffuse",
    "u_light_specular",
    "u_view_position"
};

const char* light_space_block_fields[] = {
    "u_light_space_matrix"
};

static std::string path(const char* file_path) {  // FIXME not very dry
#if defined(NINE_MORRIS_3D_DEBUG)
    // Use relative path for both operating systems
    return std::string(file_path);
#elif defined(NINE_MORRIS_3D_RELEASE)
    #if defined(NINE_MORRIS_3D_LINUX)
    std::string path = std::string("/usr/share/") + APP_NAME_LINUX + "/" + file_path;
    return path;
    #elif defined(NINE_MORRIS_3D_WINDOWS)
    // Just use relative path
    return std::string(file_path);
    #endif
#endif
}

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
    storage.light_space_uniform_buffer = UniformBuffer::create();

    storage.projection_view_uniform_block.block_name = "ProjectionView";
    storage.projection_view_uniform_block.field_count = 1;
    storage.projection_view_uniform_block.field_names = projection_view_block_fields;
    storage.projection_view_uniform_block.uniform_buffer = storage.projection_view_uniform_buffer;
    storage.projection_view_uniform_block.binding_index = 0;

    storage.light_uniform_block.block_name = "Light";
    storage.light_uniform_block.field_count = 5;
    storage.light_uniform_block.field_names = light_block_fields;
    storage.light_uniform_block.uniform_buffer = storage.light_uniform_buffer;
    storage.light_uniform_block.binding_index = 1;

    storage.light_space_uniform_block.block_name = "LightSpace";
    storage.light_space_uniform_block.field_count = 1;
    storage.light_space_uniform_block.field_names = light_space_block_fields;
    storage.light_space_uniform_block.uniform_buffer = storage.light_space_uniform_buffer;
    storage.light_space_uniform_block.binding_index = 2;

    {
        const std::vector<std::string> uniforms = {
            "u_projection_view_matrix",
            "u_skybox"
        };
        storage.skybox_shader = Shader::create(
            path(SKYBOX_VERTEX_SHADER),
            path(SKYBOX_FRAGMENT_SHADER),
            uniforms
        );
    }

    {
        const std::vector<std::string> uniforms = {
            "u_screen_texture"
        };
        storage.screen_quad_shader = Shader::create(
            path(SCREEN_QUAD_VERTEX_SHADER),
            path(SCREEN_QUAD_FRAGMENT_SHADER),
            uniforms
        );
    }

    {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_view_matrix",
            "u_projection_matrix",
            "u_texture"
        };
        storage.quad3d_shader = Shader::create(
            path(QUAD3D_VERTEX_SHADER),
            path(QUAD3D_FRAGMENT_SHADER),
            uniforms
        );
    }

    {
        const std::vector<std::string> uniforms = {
            "u_model_matrix"
        };
        storage.shadow_shader = Shader::create(
            path(SHADOW_VERTEX_SHADER),
            path(SHADOW_FRAGMENT_SHADER),
            uniforms,
            { storage.light_space_uniform_block }
        );
    }

    {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_color"
        };
        storage.outline_shader = Shader::create(
            path(OUTLINE_VERTEX_SHADER),
            path(OUTLINE_FRAGMENT_SHADER),
            uniforms,
            { storage.projection_view_uniform_block }
        );
    }

#ifdef NINE_MORRIS_3D_DEBUG
    {
        const std::vector<std::string> uniforms = {
            "u_projection_view_matrix"
        };
        storage.origin_shader = Shader::create(
            path(ORIGIN_VERTEX_SHADER),
            path(ORIGIN_FRAGMENT_SHADER),
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
        storage.screen_quad_vertex_array = VertexArray::create();
        storage.screen_quad_vertex_array->add_buffer(buffer, layout);

        VertexArray::unbind();
    }

#ifdef NINE_MORRIS_3D_DEBUG
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

#ifdef NINE_MORRIS_3D_DEBUG
    storage.light_bulb_texture = Texture::create("data/textures/internal/light_bulb/light_bulb.png", false);
#endif

    {
        FramebufferSpecification specification;
        specification.width = 2048;
        specification.height = 2048;
        specification.depth_attachment = Attachment(AttachmentFormat::DEPTH32, AttachmentType::Texture);
        specification.white_border_for_depth_texture = true;
        specification.resizable = false;

        storage.depth_map_framebuffer = Framebuffer::create(specification);

        app->purge_framebuffers();
        app->add_framebuffer(storage.depth_map_framebuffer);
    }

    {
        FramebufferSpecification specification;
        specification.width = app->app_data.width;
        specification.height = app->app_data.height;
        specification.color_attachments = {
            Attachment(AttachmentFormat::RGBA8, AttachmentType::Texture),
            Attachment(AttachmentFormat::RED_I, AttachmentType::Texture)
        };
        specification.depth_attachment = Attachment(AttachmentFormat::DEPTH24_STENCIL8,
                AttachmentType::Renderbuffer);

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
    storage.screen_quad_shader->set_uniform_int("u_screen_texture", 0);

    DEB_INFO("Initialized renderer");
}

Renderer::~Renderer() {
    DEB_INFO("Destroyed renderer");
}

void Renderer::render() {
    projection_view_matrix = app->camera.get_projection_view_matrix();

    storage.projection_view_uniform_buffer->set(&projection_view_matrix, 0);
    storage.projection_view_uniform_buffer->bind();
    storage.projection_view_uniform_buffer->upload_data();

    storage.light_uniform_buffer->set(&light.position, 0);
    storage.light_uniform_buffer->set(&light.ambient_color, 1);
    storage.light_uniform_buffer->set(&light.diffuse_color, 2);
    storage.light_uniform_buffer->set(&light.specular_color, 3);
    storage.light_uniform_buffer->set(&app->camera.get_position(), 4);
    storage.light_uniform_buffer->bind();
    storage.light_uniform_buffer->upload_data();

    setup_shadows();
    storage.depth_map_framebuffer->bind();

    clear(Depth);
    set_viewport(2048, 2048);

    // Render objects with shadows to depth buffer
    for (const auto [id, model] : models_cast_shadow) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

        storage.shadow_shader->bind();
        storage.shadow_shader->set_uniform_mat4("u_model_matrix", matrix);

        model->vertex_array->bind();

        glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
    }

    storage.scene_framebuffer->bind();

    clear(Color | Depth | Stencil);
    set_viewport(app->app_data.width, app->app_data.height);

    // Bind shadow map for use in shadow rendering
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, storage.depth_map_framebuffer->get_depth_attachment());

    // Set to zero, because we are also rendering objects with outline later
    glStencilMask(0x00);

    if (storage.skybox_texture != nullptr) {
        draw_skybox();
    }

    // Render normal models
    for (const auto [id, model] : models) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

        model->vertex_array->bind();
        model->material->bind();

        model->material->get_shader()->set_uniform_mat4("u_model_matrix", matrix);

        if (model->material->is_hoverable()) {
            glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        } else {
            glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        }

        glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
    }

    // Render models without lighting
    for (const auto [id, model] : models_no_lighting) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

        model->vertex_array->bind();
        model->material->bind();

        model->material->get_shader()->set_uniform_mat4("u_model_matrix", matrix);

        if (model->material->is_hoverable()) {
            glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        } else {
            glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        }

        glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
    }

    // Render models with outline
    for (const auto [id, model] : models_outline) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        {
            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, model->position);
            matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

            model->vertex_array->bind();
            model->material->bind();

            model->material->get_shader()->set_uniform_mat4("u_model_matrix", matrix);

            if (model->material->is_hoverable()) {
                glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            } else {
                glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            }

            glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
        }

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        {
            constexpr float SIZE = 3.6f;

            glm::mat4 matrix = glm::mat4(1.0f);
            matrix = glm::translate(matrix, model->position);
            matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(model->scale + SIZE, model->scale + SIZE,
                    model->scale + SIZE));

            storage.outline_shader->bind();
            storage.outline_shader->set_uniform_mat4("u_model_matrix", matrix);
            storage.outline_shader->set_uniform_vec3("u_color", model->outline_color);

            // Render without output to red
            glColorMaski(1, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
            glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

#ifdef NINE_MORRIS_3D_DEBUG
    if (origin) {
        draw_origin();
    }
#endif

    storage.scene_framebuffer->resolve_framebuffer(storage.intermediate_framebuffer->get_id(),
            app->app_data.width, app->app_data.height);

    storage.intermediate_framebuffer->bind();

    const int x = static_cast<int>(input::get_mouse_x());
    const int y = app->app_data.height - static_cast<int>(input::get_mouse_y());
    reader.read(1, x, y);

    Framebuffer::bind_default();

    clear(Color);
    draw_screen_quad(storage.intermediate_framebuffer->get_color_attachment(0));

    int* data;
    reader.get<int>(&data);
    hovered_id = *data;

    check_hovered_id(x, y);
}

void Renderer::add_model(Model& model, int options) {
    static unsigned int id = 0;

    const bool no_lighting = options & static_cast<int>(NoLighting);
    const bool with_outline = options & static_cast<int>(WithOutline);
    const bool cast_shadow = options & static_cast<int>(CastShadow);
    const bool has_shadow = options & static_cast<int>(HasShadow);

    model.handle = ++id;

    if (!with_outline) {
        if (no_lighting) {
            models_no_lighting[id] = &model;
        } else {
            models[id] = &model;
        }
    } else {
        models_outline[id] = &model;
    }

    if (cast_shadow) {
        models_cast_shadow[id] = &model;
    }

    if (has_shadow) {
        models_has_shadow[id] = &model;
    }

    DEB_WARN("ADDED MODEL");
}

void Renderer::remove_model(unsigned int handle) {
    models.erase(handle);
    models_no_lighting.erase(handle);
    models_outline.erase(handle);
    models_cast_shadow.erase(handle);
    models_has_shadow.erase(handle);
}

void Renderer::update_model(Model& model, int options) {
    remove_model(model.handle);

    const bool no_lighting = options & static_cast<int>(NoLighting);
    const bool with_outline = options & static_cast<int>(WithOutline);
    const bool cast_shadow = options & static_cast<int>(CastShadow);
    const bool has_shadow = options & static_cast<int>(HasShadow);

    if (!with_outline) {
        if (no_lighting) {
            models_no_lighting[model.handle] = &model;
        } else {
            models[model.handle] = &model;
        }
    } else {
        models_outline[model.handle] = &model;
    }

    if (cast_shadow) {
        models_cast_shadow[model.handle] = &model;
    }

    if (has_shadow) {
        models_has_shadow[model.handle] = &model;
    }
}

void Renderer::clear_models() {
    models.clear();
    models_no_lighting.clear();
    models_outline.clear();
    models_cast_shadow.clear();
    models_has_shadow.clear();
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

void Renderer::clear(int buffers) {
    glClear(buffers);
}

void Renderer::draw_screen_quad(GLuint texture) {
    storage.screen_quad_shader->bind();
    storage.screen_quad_vertex_array->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

#ifdef NINE_MORRIS_3D_DEBUG
void Renderer::draw_origin() {
    storage.origin_shader->bind();
    storage.origin_shader->set_uniform_mat4("u_projection_view_matrix", projection_view_matrix);

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
    storage.skybox_shader->set_uniform_mat4("u_projection_view_matrix", projection * view);

    storage.skybox_vertex_array->bind();
    storage.skybox_texture->bind(0);

    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, 36);  // TODO maybe improve
    glDepthMask(GL_TRUE);
}

void Renderer::setup_shadows() {
    const glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 9.0f);  // TODO maybe redo this stuff
    const glm::mat4 view = glm::lookAt(light.position / 4.0f,
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 light_space_matrix = projection * view;

    storage.light_space_uniform_buffer->set(&light_space_matrix, 0);
    storage.light_space_uniform_buffer->bind();
    storage.light_space_uniform_buffer->upload_data();

    for (const auto& [id, model] : models_has_shadow) {
        model->material->get_shader()->bind();
        model->material->get_shader()->set_uniform_int("u_shadow_map", 2);
    }
}

void Renderer::check_hovered_id(int x, int y) {
    if (x > app->app_data.width || x < 0 || y > app->app_data.height || y < 0) {
        hovered_id = hoverable::null;
    }
}
