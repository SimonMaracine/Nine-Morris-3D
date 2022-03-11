#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "application/events.h"
#include "graphics/renderer/shader.h"
#include "graphics/renderer/vertex_array.h"
#include "graphics/renderer/texture.h"
#include "graphics/renderer/font.h"

class Application;
class GuiRenderer;

namespace gui {
    enum Sticky {
        None,
        N, S, E, W,
        NE, NW, SE, SW
    };

    class Widget {
    public:
        Widget(std::shared_ptr<Widget> parent);
        virtual ~Widget() = default;

        virtual void render() = 0;

        Widget* set(unsigned int row, unsigned int column);
        Widget* span(unsigned int row_span, unsigned int column_span);
        Widget* padd(const glm::ivec2& padd_x, const glm::ivec2& padd_y);
        Widget* stick(Sticky sticky);

        const glm::ivec2& get_position() { return position; }
        const glm::ivec2& get_size() { return size; }
        bool get_visible() { return visible; }
        unsigned int get_row() { return row; }
        unsigned int get_column() { return column; }
        unsigned int get_row_span() { return row_span; }
        unsigned int get_column_span() { return column_span; }
        const glm::ivec2& get_padd_x() { return padd_x; }
        const glm::ivec2& get_padd_y() { return padd_y; }
        Sticky get_sticky() { return sticky; }
    protected:
        std::weak_ptr<Widget> parent;

        glm::ivec2 position = glm::ivec2(0);
        glm::ivec2 size = glm::ivec2(0);  // Width-height
        bool visible = true;
        unsigned int row = 0;
        unsigned int column = 0;
        unsigned int row_span = 1;
        unsigned int column_span = 1;
        glm::ivec2 padd_x = glm::ivec2(0);  // Left-right
        glm::ivec2 padd_y = glm::ivec2(0);  // Top-bottom
        Sticky sticky = None;

        static Application* app;

        friend class ::Application;
        friend class ::GuiRenderer;
        friend class Frame;
    };

    class Frame : public Widget {
    public:
        Frame(std::shared_ptr<Frame> parent);
        virtual ~Frame() = default;

        virtual void render() override;

        void on_window_resized(events::WindowResizedEvent& event);

        void add(std::shared_ptr<Widget> widget, unsigned int row, unsigned int column,
                unsigned int row_span = 1, unsigned int column_span = 1,
                const glm::ivec2& padd_x = glm::ivec2(0), const glm::ivec2& padd_y = glm::ivec2(0),
                Sticky sticky = None);
    private:
        struct Cell {
            std::shared_ptr<Widget> widget;
            glm::ivec2 size = glm::ivec2(0);
            glm::ivec2 position = glm::ivec2(0);
        };

        std::vector<std::shared_ptr<Widget>> children;
        unsigned int rows = 0;  // Start with 0
        unsigned int columns = 0;
        bool base = false;
    };

    class Image : public Widget {
    public:
        Image(std::shared_ptr<Frame> parent, std::shared_ptr<Texture> texture);
        virtual ~Image() = default;

        virtual void render() override;
    public:
        std::shared_ptr<Texture> texture;
    };

    class Text : public Widget {
    public:
        Text(std::shared_ptr<Frame> parent, std::shared_ptr<Font> font);
        virtual ~Text() = default;

        virtual void render() override;
    public:
        std::shared_ptr<Font> font;
    };

    class Dummy : public Widget {
    public:
        Dummy(std::shared_ptr<Frame> parent);
        virtual ~Dummy() = default;

        virtual void render() override;
    };
}

class GuiRenderer {
public:
    GuiRenderer(Application* app);
    ~GuiRenderer();

    void render();

    void im_draw_quad(const glm::vec2& position, const glm::vec2& scale,
            std::shared_ptr<Texture> texture);

    void on_window_resized(events::WindowResizedEvent& event);

    std::shared_ptr<gui::Frame> get_main_frame() { return main_frame; }
private:
    struct Storage {
        std::shared_ptr<Shader> quad2d_shader;
        std::shared_ptr<Shader> text_shader;

        std::shared_ptr<VertexArray> quad2d_vertex_array;

        glm::mat4 orthographic_projection_matrix = glm::mat4(1.0f);
    } storage;

    std::shared_ptr<gui::Frame> main_frame;
    std::shared_ptr<Texture> FrameTextureTest;  // TODO remove

    const char* QUAD2D_VERTEX_SHADER = "data/shaders/internal/quad2d.vert";
    const char* QUAD2D_FRAGMENT_SHADER = "data/shaders/internal/quad2d.frag";
    const char* TEXT_VERTEX_SHADER = "data/shaders/internal/text.vert";
    const char* TEXT_FRAGMENT_SHADER = "data/shaders/internal/text.frag";

    // Reference to application
    Application* app = nullptr;

    friend class Application;
    friend class gui::Widget;
    friend class gui::Image;
    friend class gui::Text;
    friend class gui::Frame;
};
