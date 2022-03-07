#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

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

    Widget* Widget::set(unsigned int row, unsigned int column) {
        this->row = row;
        this->column = column;
        return this;
    }

    Widget* Widget::span(unsigned int row_span, unsigned int column_span) {
        this->row_span = row_span;
        this->column_span = column_span;
        return this;
    }

    Widget* Widget::padd(const glm::ivec2& padd_x, const glm::ivec2& padd_y) {
        this->padd_x = padd_x;
        this->padd_y = padd_y;
        return this;
    }

    Widget* Widget::stick(Sticky sticky) {
        this->sticky = sticky;
        return this;
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

    Frame::Frame(std::shared_ptr<Frame> parent)
        : Widget(parent) {
        if (parent == nullptr) {
            base = true;
            size.x = app->app_data.width;
            size.y = app->app_data.height;
        }

        // TODO initialize width, height and position
    }

    void Frame::render() {
        // for (std::shared_ptr<Widget> widget : children) {
        //     widget->render();
        // }

        std::vector<Cell> cells;

        // Calculate number of rows and columns
        {
            std::unordered_set<unsigned int> row_indices;
            std::unordered_set<unsigned int> column_indices;

            for (std::shared_ptr<Widget> widget : children) {
                row_indices.insert(widget->row);
                column_indices.insert(widget->column);
            }

            rows = row_indices.size();
            columns = column_indices.size();
        }

        // Calculate normal size of cells
        for (std::shared_ptr<Widget> widget : children) {
            cells.push_back({
                widget,
                glm::ivec2(
                    widget->padd_x.x + widget->size.x + widget->padd_x.y,
                    widget->padd_y.x + widget->size.y + widget->padd_y.y
                )
            });
        }

        // Calculate normal width and height of cells together
        unsigned int normal_cells_width = 0;
        unsigned int normal_cells_height = 0;

        for (Cell& cell : cells) {
            normal_cells_width += cell.size.x;
            normal_cells_height += cell.size.y;
        }

        // Calculate actual width of each cell
        if (normal_cells_width > size.x) {
            // Width of all cells together is higher than the width of frame
            // All cells will get equal amount of width
            int WIDTH = normal_cells_width / rows;
            int REMAINING = normal_cells_width % rows;

            for (Cell& cell : cells) {
                cell.size.x = WIDTH;
            }
            cells[0].size.x += REMAINING;
        } else {
            // There is more space than needed
            // All cells will get an equal amount of additional width
            int ADD_WIDTH = (size.x - normal_cells_width) / rows;
            int ADD_REMAINING = (size.x - normal_cells_width) % rows;

            for (Cell& cell : cells) {
                cell.size.x += ADD_WIDTH;
            }
            cells[0].size.x += ADD_REMAINING;
        }

        // Calculate actual height of each cell
        if (normal_cells_height > size.y) {
            // Height of all cells together is higher than the height of frame
            // All cells will get equal amount of height
            int HEIGHT = normal_cells_height / columns;
            int REMAINING = normal_cells_height % columns;

            for (Cell& cell : cells) {
                cell.size.y = HEIGHT;
            }
            cells[0].size.y += REMAINING;
        } else {
            // There is more space than needed
            // All cells will get an equal amount of additional height
            int ADD_HEIGHT = (size.y - normal_cells_height) / columns;
            int ADD_REMAINING = (size.y - normal_cells_height) % columns;

            for (Cell& cell : cells) {
                cell.size.y += ADD_HEIGHT;
            }
            cells[0].size.y += ADD_REMAINING;
        }

        // Reorganize the cells in a matrix
        std::vector<std::vector<Cell>> matrix;

        std::stable_sort(cells.begin(), cells.end(), [&cells](const Cell& lhs, const Cell& rhs) {
            if (lhs.widget->column < rhs.widget->column) {
                if (lhs.widget->row < rhs.widget->row) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        });

        for (unsigned int i = 0; i < rows; i++) {
            matrix.push_back(std::vector<Cell>());

            for (unsigned int j = 0; j < columns; j++) {
                for (const Cell& cell : cells) {
                    // matrix[i].push_back({

                    // });

                    // if (cell.widget->row)
                }
            }
        }

        // Finish setting the size for each cell
        if (rows > 1) {
            // Iterate every column
            // for ()
        }

        // Calculate position of each widget in its cell
        for (auto& [widget, size] : cells) {
            switch (widget->sticky) {
                case None:  // Center the widget both ways

                    break;
                case N:
                    break;
                case S:
                    break;
                case E:
                    break;
                case W:
                    break;
                case NE:
                    break;
                case NW:
                    break;
                case SE:
                    break;
                case SW:
                    break;
            }
        }



    }

    void Frame::add(std::shared_ptr<Widget> widget, unsigned int row, unsigned int column,
            unsigned int row_span, unsigned int column_span, const glm::ivec2& padd_x,
            const glm::ivec2& padd_y, Sticky sticky) {
        widget->set(row, column)->span(row_span, column_span)->padd(padd_x, padd_y)->stick(sticky);
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
    main_frame = std::make_shared<gui::Frame>(nullptr);

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

void GuiRenderer::im_draw_quad(const glm::vec2& position, const glm::vec2& scale,
            std::shared_ptr<Texture> texture) {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(position, 0.0f));
    matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, 1.0f));

    storage.quad2d_shader->bind();
    storage.quad2d_shader->set_uniform_mat4("u_model_matrix", matrix);

    texture->bind(0);
    storage.quad2d_vertex_array->bind();

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}