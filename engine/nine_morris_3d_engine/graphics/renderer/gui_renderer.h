#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/graphics/font.h"
#include "nine_morris_3d_engine/graphics/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.h"
#include "nine_morris_3d_engine/graphics/opengl/texture.h"
#include "nine_morris_3d_engine/graphics/opengl/buffer.h"
#include "nine_morris_3d_engine/other/encrypt.h"

class Application;
class GuiRenderer;

namespace gui {
    enum class WidgetType {
        None,
        Image,
        Text
    };

    enum class Sticky {
        Center,
        N, S, E, W,
        NE, NW, SE, SW
    };

    enum class Relative {
        Left, Right, Top, Bottom
    };

    class Widget {
    public:
        Widget() = default;
        virtual ~Widget() = default;

        virtual void render() = 0;

        Widget* offset(int offset, Relative relative);
        Widget* stick(Sticky sticky);
        Widget* scale(float min_scale, float max_scale, int min_bound, int max_bound);
        Widget* fake_size(glm::vec2 fake_size);

        glm::vec2 get_position() { return position; }
        glm::vec2 get_size() { return size; }
        Sticky get_sticky() { return sticky; }
        glm::vec2 get_actual_size() { return size * scale_parameters.current_scale; }
        float get_current_scale() { return scale_parameters.current_scale; }
    protected:
        glm::vec2 position = glm::vec2(0.0f);  // Relative to bottom-left
        glm::vec2 size = glm::vec2(0.0f);  // Width-height

        struct {
            int left = 0;
            int right = 0;
            int top = 0;
            int bottom = 0;
        } offset_parameters;

        struct {
            float min_scale = 1.0f;
            float max_scale = 1.0f;
            int min_bound = 0;
            int max_bound = 0;
            float current_scale = 1.0f;
        } scale_parameters;

        struct {
            glm::vec2 fake_size = glm::vec2(0.0f);
            bool fake = false;
        } fake;

        Sticky sticky = Sticky::Center;

        WidgetType type = WidgetType::None;

        static Application* app;

        friend class ::Application;
        friend class ::GuiRenderer;
    };

    class Image : public Widget {
    public:
        Image(std::shared_ptr<Texture> texture);
        virtual ~Image() = default;

        virtual void render() override;

        void set_image(std::shared_ptr<Texture> texture);
        void set_position(glm::vec2 position);
        void set_size(glm::vec2 size);
    private:
        std::shared_ptr<Texture> texture;
    };

    class Text : public Widget {
    public:
        Text(std::shared_ptr<Font> font, std::string_view text, float text_scale = 1.0f,
            const glm::vec3& color = glm::vec3(1.0f));
        virtual ~Text() = default;

        virtual void render() override;

        void set_text(std::string_view text);
        void set_scale(float text_scale);
        void set_color(const glm::vec3& color);
        void set_shadows(bool enable);
    private:
        std::shared_ptr<Font> font;
        std::string text;
        float text_scale = 1.0f;
        glm::vec3 color = glm::vec3(0.0f);
        bool with_shadows = false;
    };
}

class GuiRenderer {
public:
    GuiRenderer(Application* app);
    ~GuiRenderer();

    GuiRenderer(const GuiRenderer&) = delete;
    GuiRenderer& operator=(const GuiRenderer&) = delete;
    GuiRenderer(GuiRenderer&&) = delete;
    GuiRenderer& operator=(GuiRenderer&&) = delete;

    void render();

    void add_widget(std::shared_ptr<gui::Widget> widget);
    void remove_widget(std::shared_ptr<gui::Widget> widget);

    void clear();

    void quad_center(float& width, float& height, float& x_pos, float& y_pos);

    std::shared_ptr<Shader> get_quad2d_shader() { return storage.quad2d_shader; }
    std::shared_ptr<Shader> get_text_shader() { return storage.text_shader; }
private:
    void prepare_draw_image();
    void prepare_draw_text();
    void draw(std::vector<gui::Widget*>& subwidgets, const std::function<void()>& prepare_draw);
    void on_window_resized(const WindowResizedEvent& event);

    struct Storage {
        std::shared_ptr<UniformBuffer> projection_uniform_buffer;

        UniformBlockSpecification projection_uniform_block;

        std::shared_ptr<Shader> quad2d_shader;
        std::shared_ptr<Shader> text_shader;

        std::shared_ptr<VertexArray> quad2d_vertex_array;
        std::shared_ptr<Buffer> quad2d_buffer;

        glm::mat4 orthographic_projection_matrix = glm::mat4(1.0f);
    } storage;

    std::vector<std::shared_ptr<gui::Widget>> widgets;

    const char* QUAD2D_VERTEX_SHADER = ENCR("engine_data/shaders/quad2d.vert");
    const char* QUAD2D_FRAGMENT_SHADER = ENCR("engine_data/shaders/quad2d.frag");
    const char* TEXT_VERTEX_SHADER = ENCR("engine_data/shaders/text.vert");
    const char* TEXT_FRAGMENT_SHADER = ENCR("engine_data/shaders/text.frag");

    Application* app = nullptr;

    friend class Application;
    friend class gui::Widget;
    friend class gui::Image;
    friend class gui::Text;
};
