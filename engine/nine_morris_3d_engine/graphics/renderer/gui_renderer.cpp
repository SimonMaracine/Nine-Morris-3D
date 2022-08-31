#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "nine_morris_3d_engine/application/application.h"
#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/font.h"
#include "nine_morris_3d_engine/graphics/renderer/buffer_layout.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/buffer.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/texture.h"
#include "nine_morris_3d_engine/other/paths.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encryption.h"

static float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

namespace gui {
    Widget* Widget::offset(unsigned int offset, Relative relative) {
        switch (relative) {
            case Relative::Left:
                offset_parameters.left = offset;
                break;
            case Relative::Right:
                offset_parameters.right = offset;
                break;
            case Relative::Top:
                offset_parameters.top = offset;
                break;
            case Relative::Bottom:
                offset_parameters.bottom = offset;
                break;
        }

        return this;
    }

    Widget* Widget::stick(Sticky sticky) {
        this->sticky = sticky;

        return this;
    }

    Widget* Widget::scale(float min_scale, float max_scale, int min_bound, int max_bound) {
        scale_parameters.min_scale = min_scale;
        scale_parameters.max_scale = max_scale;
        scale_parameters.min_bound = min_bound;
        scale_parameters.max_bound = max_bound;

        return this;
    }

    Application* Widget::app = nullptr;

    Image::Image(std::shared_ptr<Texture> texture)
        : texture(texture) {
        type = WidgetType::Image;
        size.x = texture->get_width();
        size.y = texture->get_height();
    }

    void Image::render() {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(static_cast<glm::vec2>(size) * scale_parameters.current_scale, 1.0f));

        app->gui_renderer->storage.quad2d_shader->upload_uniform_mat4("u_model_matrix", matrix);

        texture->bind(0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Image::set_image(std::shared_ptr<Texture> texture) {
        this->texture = texture;
        size.x = texture->get_width();
        size.y = texture->get_height();
    }

    Text::Text(std::shared_ptr<Font> font, std::string_view text, float text_scale, const glm::vec3& color)
        : font(font), text(text), text_scale(text_scale), color(color) {
        type = WidgetType::Text;
        font->get_string_size(text, text_scale, &size.x, &size.y);
    }

    void Text::render() {
        size_t size;
        float* buffer;
        font->render(text, &size, &buffer);

        font->update_data(buffer, size);
        delete[] buffer;

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(text_scale, text_scale, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale_parameters.current_scale, scale_parameters.current_scale, 1.0f));

        app->gui_renderer->storage.text_shader->upload_uniform_mat4("u_model_matrix", matrix);
        app->gui_renderer->storage.text_shader->upload_uniform_vec3("u_color", color);
        if (!with_shadows) {
            app->gui_renderer->storage.text_shader->upload_uniform_float("u_border_width", 0.0f);
            app->gui_renderer->storage.text_shader->upload_uniform_vec2("u_offset", glm::vec2(0.0f, 0.0f));
        } else {
            app->gui_renderer->storage.text_shader->upload_uniform_float("u_border_width", 0.3f);
            app->gui_renderer->storage.text_shader->upload_uniform_vec2("u_offset", glm::vec2(-0.003f, -0.003f));
        }

        font->get_vertex_array()->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font->get_texture());

        glDrawArrays(GL_TRIANGLES, 0, font->get_vertex_count());
    }

    void Text::set_text(std::string_view text) {
        this->text = text;
        font->get_string_size(text, text_scale, &size.x, &size.y);
    }

    void Text::set_scale(float text_scale) {
        this->text_scale = text_scale;
        font->get_string_size(text, text_scale, &size.x, &size.y);
    }

    void Text::set_color(const glm::vec3& color) {
        this->color = color;
    }

    void Text::set_shadows(bool enable) {
        with_shadows = enable;
    }
}

GuiRenderer::GuiRenderer(Application* app)
    : app(app) {
    storage.projection_uniform_buffer = UniformBuffer::create();

    storage.projection_uniform_block.block_name = "Projection";
    storage.projection_uniform_block.field_count = 1;
    storage.projection_uniform_block.field_names = { "u_projection_matrix" };
    storage.projection_uniform_block.uniform_buffer = storage.projection_uniform_buffer;
    storage.projection_uniform_block.binding_index = 4;

    maybe_initialize_assets();

    using namespace encryption;

    {
        storage.quad2d_shader = Shader::create(
            encr(paths::path_for_assets(QUAD2D_VERTEX_SHADER)),
            encr(paths::path_for_assets(QUAD2D_FRAGMENT_SHADER)),
            { "u_model_matrix", "u_texture" },
            { storage.projection_uniform_block }
        );
    }

    {
        const std::vector<std::string> uniforms = {
            "u_model_matrix",
            "u_bitmap",
            "u_color",
            "u_border_width",
            "u_offset"
        };
        storage.text_shader = Shader::create(
            encr(paths::path_for_assets(TEXT_VERTEX_SHADER)),
            encr(paths::path_for_assets(TEXT_FRAGMENT_SHADER)),
            uniforms,
            { storage.projection_uniform_block }
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
    storage.projection_uniform_buffer->set(&storage.orthographic_projection_matrix, 0);
    storage.projection_uniform_buffer->bind();
    storage.projection_uniform_buffer->upload_data();

    storage.quad2d_shader->bind();
    storage.quad2d_shader->upload_uniform_int("u_texture", 0);

    Shader::unbind();

    // Set renderer pointer to widgets
    gui::Widget::app = app;

    DEB_INFO("Initialized GUI renderer");
}

GuiRenderer::~GuiRenderer() {
    DEB_INFO("Destroyed GUI renderer");
}

void GuiRenderer::render() {
    static std::vector<gui::Widget*> images;
    static std::vector<gui::Widget*> texts;

    images.clear();
    texts.clear();

    std::for_each(widgets.begin(), widgets.end(), [&](const std::shared_ptr<gui::Widget>& widget) {
        switch (widget->type) {
            case gui::WidgetType::Image:
                images.push_back(widget.get());
                break;
            case gui::WidgetType::Text:
                texts.push_back(widget.get());
                break;
            case gui::WidgetType::None:
                ASSERT(false, "Widget type must not be None");
                break;
        }
    });

    glDisable(GL_DEPTH_TEST);
    draw(images, std::bind(&GuiRenderer::prepare_draw_image, this));
    draw(texts, std::bind(&GuiRenderer::prepare_draw_text, this));
    glEnable(GL_DEPTH_TEST);
}

void GuiRenderer::im_draw_quad(glm::vec2 position, glm::vec2 scale, std::shared_ptr<Texture> texture) {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
    matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, 1.0f));

    storage.quad2d_shader->bind();
    storage.quad2d_shader->upload_uniform_mat4("u_model_matrix", matrix);

    storage.quad2d_vertex_array->bind();
    texture->bind(0);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

void GuiRenderer::on_window_resized(const WindowResizedEvent& event) {
    storage.orthographic_projection_matrix = glm::ortho(
        0.0f, static_cast<float>(event.width),
        0.0f, static_cast<float>(event.height)
    );

    storage.projection_uniform_buffer->set(&storage.orthographic_projection_matrix, 0);
    storage.projection_uniform_buffer->bind();
    storage.projection_uniform_buffer->upload_data();
}

void GuiRenderer::add_widget(std::shared_ptr<gui::Widget> widget) {
    auto iter = std::find(widgets.begin(), widgets.end(), widget);

    if (iter == widgets.end()) {
        widgets.push_back(widget);
    } else {
        DEB_WARN("Widget alreay present");
    }
}

void GuiRenderer::remove_widget(std::shared_ptr<gui::Widget> widget) {
    auto iter = std::find(widgets.begin(), widgets.end(), widget);

    if (iter != widgets.end()) {
        widgets.erase(iter);
    }
}

void GuiRenderer::clear() {
    widgets.clear();
}

void GuiRenderer::prepare_draw_image() {
    app->gui_renderer->storage.quad2d_shader->bind();
    app->gui_renderer->storage.quad2d_vertex_array->bind();
}

void GuiRenderer::prepare_draw_text() {
    app->gui_renderer->storage.text_shader->bind();
}

void GuiRenderer::draw(std::vector<gui::Widget*>& subwidgets, const std::function<void()>& prepare_draw) {
    prepare_draw();

    for (gui::Widget* widget : subwidgets) {
        const int WINDOW_WIDTH = app->app_data.width;
        const int WINDOW_HEIGHT = app->app_data.height;

        if (widget->scale_parameters.min_bound != 0 && widget->scale_parameters.max_bound != 0) {
            if (WINDOW_HEIGHT <= widget->scale_parameters.min_bound) {
                widget->scale_parameters.current_scale = widget->scale_parameters.min_scale;
            } else if (WINDOW_HEIGHT >= widget->scale_parameters.max_bound) {
                widget->scale_parameters.current_scale = widget->scale_parameters.max_scale;
            } else {
                widget->scale_parameters.current_scale = map(
                    WINDOW_HEIGHT,
                    widget->scale_parameters.min_bound,
                    widget->scale_parameters.max_bound,
                    widget->scale_parameters.min_scale,
                    widget->scale_parameters.max_scale
                );
            }
        }

        const float SCALE = widget->scale_parameters.current_scale;

        switch (widget->sticky) {
            case gui::Sticky::Center:
                widget->position = glm::ivec2(
                    WINDOW_WIDTH / 2 - widget->size.x * SCALE / 2,
                    WINDOW_HEIGHT / 2 - widget->size.y * SCALE / 2
                );
                break;
            case gui::Sticky::N:
                widget->position = glm::ivec2(
                    WINDOW_WIDTH / 2 - widget->size.x * SCALE / 2,
                    WINDOW_HEIGHT - widget->size.y * SCALE - widget->offset_parameters.top
                );
                break;
            case gui::Sticky::S:
                widget->position = glm::ivec2(
                    WINDOW_WIDTH / 2 - widget->size.x * SCALE / 2,
                    widget->offset_parameters.bottom
                );
                break;
            case gui::Sticky::E:
                widget->position = glm::ivec2(
                    WINDOW_WIDTH - widget->size.x * SCALE - widget->offset_parameters.right,
                    WINDOW_HEIGHT / 2 - widget->size.x * SCALE / 2
                );
                break;
            case gui::Sticky::W:
                widget->position = glm::ivec2(
                    widget->offset_parameters.left,
                    WINDOW_HEIGHT / 2 - widget->size.x * SCALE / 2
                );
                break;
            case gui::Sticky::NE:
                widget->position = glm::ivec2(
                    WINDOW_WIDTH - widget->size.x * SCALE - widget->offset_parameters.right,
                    WINDOW_HEIGHT - widget->size.y * SCALE - widget->offset_parameters.top
                );
                break;
            case gui::Sticky::NW:
                widget->position = glm::ivec2(
                    widget->offset_parameters.left,
                    WINDOW_HEIGHT - widget->size.y * SCALE - widget->offset_parameters.top
                );
                break;
            case gui::Sticky::SE:
                widget->position = glm::ivec2(
                    WINDOW_WIDTH - widget->size.x * SCALE - widget->offset_parameters.right,
                    widget->offset_parameters.bottom
                );
                break;
            case gui::Sticky::SW:
                widget->position = glm::ivec2(
                    widget->offset_parameters.left,
                    widget->offset_parameters.bottom
                );
                break;
        }

        widget->render();
    }
}

void GuiRenderer::maybe_initialize_assets() {
#ifdef PLATFORM_GAME_RELEASE
    static const char* PREFIX = ".dat";

    static const std::array<std::string*, 4> assets = { 
        &QUAD2D_VERTEX_SHADER,
        &QUAD2D_FRAGMENT_SHADER,
        &TEXT_VERTEX_SHADER,
        &TEXT_FRAGMENT_SHADER
    };

    for (std::string* asset : assets) {
        *asset += PREFIX;
    }
#endif
}
