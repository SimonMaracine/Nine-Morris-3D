#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "graphics/renderer/gui_renderer.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/buffer.h"
#include "graphics/renderer/buffer_layout.h"
#include "other/logging.h"

static std::string path(const char* file_path) {  // FIXME NOT DRY AAAAAHHH
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

GuiRenderer::GuiRenderer(Application* app)
    : app(app) {
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

    storage.orthographic_projection_matrix = glm::ortho(
        0.0f, static_cast<float>(app->app_data.width),
        0.0f, static_cast<float>(app->app_data.height)
    );

    DEB_INFO("Initialized GUI renderer");
}

GuiRenderer::~GuiRenderer() {
    DEB_INFO("Destroyed GUI renderer");
}

void GuiRenderer::render() {

}
