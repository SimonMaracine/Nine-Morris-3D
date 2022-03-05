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
#include "graphics/renderer/texture.h"
#include "graphics/renderer/font.h"
#include "other/logging.h"

namespace gui {
    Widget::Widget(std::shared_ptr<Widget> parent)
        : parent(parent) {

    }

    void Widget::set(unsigned int row, unsigned int column, unsigned int row_span, unsigned int column_span,
            const glm::ivec2& padd_x, const glm::ivec2& padd_y) {
        this->row = row;
        this->column = column;
        this->row_span = row_span;
        this->column_span = column_span;
        this->padd_x = padd_x;
        this->padd_y = padd_y;
    }

    Application* Widget::app = nullptr;

    Image::Image(std::shared_ptr<Frame> parent, std::shared_ptr<Texture> texture)
        : Widget(parent), texture(texture) {
        size.x = texture->get_width();
        size.y = texture->get_height();
    }

    void Image::render() {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(size, 1.0f));

        app->gui_renderer->storage.quad2d_shader->bind();
        app->gui_renderer->storage.quad2d_shader->set_uniform_mat4("u_model_matrix", matrix);

        texture->bind(0);
        app->gui_renderer->storage.quad2d_vertex_array->bind();

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    Text::Text(std::shared_ptr<Frame> parent, std::shared_ptr<Font> font)
        : Widget(parent), font(font) {

    }

    void Text::render() {

    }

    Frame::Frame(std::shared_ptr<Frame> parent, unsigned int rows, unsigned int columns)
        : Widget(parent), rows(rows), columns(columns) {
        if (parent == nullptr) {
            base = true;
            size.x = app->app_data.width;
            size.y = app->app_data.height;
        }

        // TODO initialize width, height and position
    }

    void Frame::render() {
        for (std::shared_ptr<Widget> widget : children) {
            widget->render();
        }
    }

    void Frame::add(std::shared_ptr<Widget> widget) {
        children.push_back(widget);
    }
}

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

    // Setup uniform variables
    storage.quad2d_shader->bind();
    storage.quad2d_shader->set_uniform_mat4("u_projection_matrix", storage.orthographic_projection_matrix);
    storage.quad2d_shader->set_uniform_int("u_texture", 0);

    // Set renderer pointer to widgets
    gui::Widget::app = app;

    // Initialize main frame
    main_frame = std::make_shared<gui::Frame>(nullptr, 1, 1);

    DEB_INFO("Initialized GUI renderer");
}

GuiRenderer::~GuiRenderer() {
    DEB_INFO("Destroyed GUI renderer");
}

void GuiRenderer::render() {
    glDisable(GL_DEPTH_TEST);

    main_frame->render();

    glEnable(GL_DEPTH_TEST);
}
