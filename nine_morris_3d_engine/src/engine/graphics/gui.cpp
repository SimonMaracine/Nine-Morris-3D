#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <resmanager/resmanager.h>

#include "engine/application/application.h"
#include "engine/graphics/renderer/render_helpers.h"
#include "engine/graphics/gui.h"

using namespace resmanager::literals;

namespace gui {
    Widget* Widget::offset(int offset, Relative relative) {
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

    Widget* Widget::fake_size(glm::vec2 fake_size) {
        fake.fake_size = fake_size;
        fake.fake = true;

        return this;
    }

    Application* Widget::app = nullptr;

    Image::Image(std::shared_ptr<gl::Texture> texture)
        : texture(texture) {
        type = WidgetType::Image;
        size.x = texture->get_width();
        size.y = texture->get_height();
    }

    void Image::render() {
        ctx->r2d->draw_quad(position, size * scale_parameters.current_scale, texture);
    }

    void Image::set_image(std::shared_ptr<gl::Texture> texture) {
        this->texture = texture;
        size.x = texture->get_width();
        size.y = texture->get_height();
    }

    void Image::set_position(glm::vec2 position) {
        this->position = position;
    }

    void Image::set_size(glm::vec2 size) {
        this->size = size;
    }

    Text::Text(std::shared_ptr<Font> font, std::string_view text, float text_scale, const glm::vec3& color)
        : font(font), text(text), text_scale(text_scale), color(color) {
        type = WidgetType::Text;

        const auto [x, y] = font->get_string_size(text, text_scale);

        size.x = static_cast<float>(x);
        size.y = static_cast<float>(y);
    }

    void Text::render() {
        static std::vector<float> buffer;
        buffer.clear();

        font->render(text, buffer);
        font->update_data(buffer.data(), sizeof(float) * buffer.size());

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(text_scale, text_scale, 1.0f));
        matrix = glm::scale(matrix, glm::vec3(scale_parameters.current_scale, scale_parameters.current_scale, 1.0f));

        ctx->r2d->storage.text_shader->upload_uniform_mat4("u_model_matrix"_H, matrix);
        ctx->r2d->storage.text_shader->upload_uniform_vec3("u_color"_H, color);

        const float border_width = with_shadows ? 0.3f : 0.0f;
        const float offset = with_shadows ? -0.003f : 0.0f;

        ctx->r2d->storage.text_shader->upload_uniform_float("u_border_width"_H, border_width);
        ctx->r2d->storage.text_shader->upload_uniform_vec2("u_offset"_H, glm::vec2(offset, offset));

        font->get_vertex_array()->bind();

        render_helpers::bind_texture_2d(font->get_bitmap()->get_id(), 0);

        render_helpers::draw_arrays(font->get_vertex_count());
    }

    void Text::set_text(std::string_view text) {
        this->text = text;

        const auto [x, y] = font->get_string_size(text, text_scale);

        size.x = static_cast<float>(x);
        size.y = static_cast<float>(y);
    }

    void Text::set_scale(float text_scale) {
        this->text_scale = text_scale;

        const auto [x, y] = font->get_string_size(text, text_scale);

        size.x = static_cast<float>(x);
        size.y = static_cast<float>(y);
    }

    void Text::set_color(const glm::vec3& color) {
        this->color = color;
    }

    void Text::set_shadows(bool enable) {
        with_shadows = enable;
    }
}
