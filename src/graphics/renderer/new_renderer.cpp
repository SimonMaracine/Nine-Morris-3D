#include <memory>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_truetype.h>

#include "application/application.h"
#include "application/platform.h"
#include "graphics/renderer/new_renderer.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/framebuffer.h"
#include "other/logging.h"

std::string path(const char* file_path) {  // FIXME not very dry
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
    // glEnable(GL_STENCIL_TEST);
    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
            "u_model_matrix",
            "u_projection_matrix",
            "u_texture"
        };
        storage.quad2d_shader = Shader::create(
            path(QUAD2D_VERTEX_SHADER),
            path(QUAD2D_FRAGMENT_SHADER),
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
            "u_model_matrix",
            "u_light_space_matrix"
        };
        storage.shadow_shader = Shader::create(
            path(SHADOW_VERTEX_SHADER),
            path(SHADOW_FRAGMENT_SHADER),
            uniforms
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
            uniforms
            // block_name, 1,
            // storage.uniform_buffer
        );
    }

    {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_projection_matrix",
            "u_bitmap",
            "u_color",
            "u_border_width",
            "u_offset"
        };
        storage.text_shader = Shader::create(
            path(TEXT_VERTEX_SHADER),
            path(TEXT_FRAGMENT_SHADER),
            uniforms
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
        float quad2d_vertices[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };

        std::shared_ptr<Buffer> buffer = Buffer::create(quad2d_vertices, sizeof(quad2d_vertices));
        BufferLayout layout;
        layout.add(0, BufferLayout::Type::Float, 2);
        storage.quad2d_vertex_array = VertexArray::create();
        storage.quad2d_vertex_array->add_buffer(buffer, layout);

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

        storage.intermediate_framebuffer = Framebuffer::create(specification);  // TODO don't know

        app->purge_framebuffers();
        app->add_framebuffer(storage.intermediate_framebuffer);
    }

    storage.orthographic_projection_matrix = glm::ortho(0.0f, static_cast<float>(app->app_data.width),
                0.0f, static_cast<float>(app->app_data.height));

    DEB_INFO("Initialized renderer");
}

Renderer::~Renderer() {
    DEB_INFO("Destroyed renderer");
}

void Renderer::render() {
    projection_view_matrix = app->camera.get_projection_view_matrix();

    storage.scene_framebuffer->bind();

    clear(Color | Depth | Stencil);
    set_viewport(app->app_data.width, app->app_data.height);
    // set_stencil_mask_zero();

    if (storage.skybox_texture != nullptr) {
        draw_skybox();    
    }

    for (const auto& [id, model] : models) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

        model->material->get_shader()->bind();  // Optimize this (maybe by using uniform buffers)
        model->material->get_shader()->set_uniform_mat4("u_model_matrix", matrix);
        model->material->get_shader()->set_uniform_mat4("u_projection_view_matrix", projection_view_matrix);
        model->material->get_shader()->set_uniform_vec3("u_view_position", app->camera.get_position());

        model->material->get_shader()->set_uniform_vec3("u_light.position", light.position);
        model->material->get_shader()->set_uniform_vec3("u_light.ambient", light.ambient_color);
        model->material->get_shader()->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        model->material->get_shader()->set_uniform_vec3("u_light.specular", light.specular_color);

        model->vertex_array->bind();
        model->material->bind();
        // model->diffuse_texture->bind(0);

        glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
    }

    for (const auto& [id, model] : models_no_lighting) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, model->position);
        matrix = glm::rotate(matrix, model->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, model->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(model->scale, model->scale, model->scale));

        model->material->get_shader()->bind();
        model->material->get_shader()->set_uniform_mat4("u_model_matrix", matrix);
        model->material->get_shader()->set_uniform_mat4("u_projection_view_matrix", projection_view_matrix);

        model->vertex_array->bind();
        model->material->bind();

        glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, nullptr);
    }

#ifdef NINE_MORRIS_3D_DEBUG
    draw_origin();
#endif

    storage.scene_framebuffer->resolve_framebuffer(storage.intermediate_framebuffer->get_id(),
            app->app_data.width, app->app_data.height);

    Framebuffer::bind_default();

    clear(Color);
    draw_screen_quad(storage.intermediate_framebuffer->get_color_attachment(0));
}

unsigned int Renderer::add_model(Model& model, int options) {
    static unsigned int id = 0;

    const bool no_lighting = options & (1 << 0);
    const bool with_outline = options & (1 << 1);
    const bool with_shadow = options & (1 << 2);

    model.id = ++id;

    if (!no_lighting) {
        models[id] = &model;
    } else {
        models_no_lighting[id] = &model;
    }

    if (with_outline) {
        models_outline[id] = &model;
    }

    if (with_shadow) {
        models_shadow[id] = &model;
    }

    return id;
}

void Renderer::remove_model(unsigned int handle) {
    models.erase(handle);
    models_no_lighting.erase(handle);
    models_outline.erase(handle);
    models_shadow.erase(handle);
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

    glDrawArrays(GL_LINES, 0, 6);
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
