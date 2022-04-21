#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cassert>

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

    Frame::Frame(std::shared_ptr<Frame> parent)
        : Widget(parent) {
        if (parent == nullptr) {
            base = true;
            size.x = app->app_data.width;
            size.y = app->app_data.height;
            // Position remains (0, 0)
        }

        // TODO initialize width, height and position
    }

    void Frame::render() {
        // Exit early, if there are no children
        if (children.empty()) {
            return;
        }

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

            assert(rows > 0);
            assert(columns > 0);
        }

        // Create cells and calculate normal size for each cell
        for (std::shared_ptr<Widget> widget : children) {
            cells.push_back({
                widget,
                glm::ivec2(
                    widget->padd_x.x + widget->size.x + widget->padd_x.y,
                    widget->padd_y.x + widget->size.y + widget->padd_y.y
                ),
                glm::ivec2(0)
            });
        }

        // Sort cells
        std::stable_sort(cells.begin(), cells.end(), [](const Cell& lhs, const Cell& rhs) {
            if (lhs.widget->row < rhs.widget->row) {
                return true;
            } else if (lhs.widget->row > rhs.widget->row) {
                return false;
            } else {
                if (lhs.widget->column < rhs.widget->column) {
                    return true;
                } else if (lhs.widget->column > rhs.widget->column) {
                    return false;
                } else {
                    return false;
                }
            }
        });

        // Figure out if there are missing cells and act accordingly
        if (cells.size() != rows * columns) {
            REL_CRITICAL("There are missing cells; please fill them with gui::Dummy widgets");
            exit(1);
        }

        // Calculate normal width and height of cells together
        int normal_cells_width = 0;
        int normal_cells_height = 0;

        {
            {
                std::vector<int> row_widths;

                for (unsigned int j = 0; j < rows; j++) {
                    int row_width = 0;
                    for (unsigned int i = 0; i < cells.size() / rows; i++) {
                        row_width += cells[i + j * columns].size.x;
                    }

                    assert(row_width > 0);

                    row_widths.push_back(row_width);
                }

                // Get max width of each row
                for (int width : row_widths) {
                    normal_cells_width = std::max(normal_cells_width, width);
                }
            }

            {
                std::vector<int> column_heights;

                for (unsigned int j = 0; j < columns; j++) {
                    int column_height = 0;
                    for (unsigned int i = 0; i < cells.size(); i += columns) {
                        column_height += cells[i + j].size.y;
                    }

                    assert(column_height > 0);

                    column_heights.push_back(column_height);
                }

                // Get max height of each column
                for (int height : column_heights) {
                    normal_cells_height = std::max(normal_cells_height, height);
                }
            }

            assert(normal_cells_width > 0);
            assert(normal_cells_height > 0);
        }

        // Use the additional width to fill the width for each cell,
        // If there is more space than needed
        if (normal_cells_width < size.x) {
            // All cells will get an equal amount of additional width
            const int ADDITIONAL_WIDTH = (size.x - normal_cells_width) / columns;

            for (Cell& cell : cells) {
                cell.size.x += ADDITIONAL_WIDTH;
            }
        }

        // Use the additional height to fill the height for each cell,
        // If there is more space than needed
        if (normal_cells_height < size.y) {
            // All cells will get an equal amount of additional height
            const int ADDITIONAL_HEIGHT = (size.y - normal_cells_height) / rows;

            for (Cell& cell : cells) {
                cell.size.y += ADDITIONAL_HEIGHT;
            }
        }

        // Calculate width and height considering padding
        // {
            // for (Cell& cell : cells) {
            //     cell.size.x = std::max(
            //         cell.size.x,
            //         cell.widget->padd_x.x + cell.widget->size.x + cell.widget->padd_x.y
            //     );

            //     cell.size.y = std::max(
            //         cell.size.y,
            //         cell.widget->padd_y.x + cell.widget->size.y + cell.widget->padd_y.y
            //     );
            // }
        // }

        // Finish setting the size for each cell
        // For each column, set the width as the max width of the cells
        if (rows > 1) {
            // Iterate every column
            for (unsigned int j = 0; j < columns; j++) {
                int max_width = 0;

                for (unsigned int i = 0; i < rows; i++) {
                    max_width = std::max(cells[j + i * columns].size.x, max_width);
                }

                for (unsigned int i = 0; i < rows; i++) {
                    cells[j + i * columns].size.x = max_width;
                }
            }
        }

        // For each row, set the height as the max height of the cells
        if (columns > 1) {
            // Iterate every row
            for (unsigned int j = 0; j < rows; j++) {
                int max_height = 0;

                for (unsigned int i = 0; i < columns; i++) {
                    max_height = std::max(cells[i + j * columns].size.y, max_height);
                }

                for (unsigned int i = 0; i < columns; i++) {
                    cells[i + j * columns].size.y = max_height;  // TODO don't reset size, if 
                }
            }
        }

        // Calculate position of each cell in the grid
        {
            int position_x = 0;
            unsigned int x_index = 0;

            int position_y = 0;
            unsigned int y_index = 0;

            for (Cell& cell : cells) {
                // x position
                cell.position.x = position_x;
                position_x += cell.size.x;

                x_index++;

                if (x_index == columns) {
                    position_x = 0;
                    x_index = 0;
                }

                // y position
                if (y_index % columns == 0) {
                    position_y += cell.size.y;
                }

                cell.position.y = size.y - position_y;

                y_index++;
            }
        }

        // Calculate position of each widget in its cell
        for (Cell& cell : cells) {
            switch (cell.widget->sticky) {
                case None:  // Center the widget both ways
                    cell.widget->position.x = cell.position.x + cell.size.x / 2
                            - cell.widget->size.x / 2;
                    cell.widget->position.y = cell.position.y + cell.size.y / 2
                            - cell.widget->size.y / 2;
                    break;
                case N:
                    cell.widget->position.x = cell.position.x + cell.size.x / 2
                            - cell.widget->size.x / 2;
                    cell.widget->position.y = cell.position.y + cell.size.y
                            - cell.widget->size.y - cell.widget->padd_y.x;
                    break;
                case S:
                    cell.widget->position.x = cell.position.x + cell.size.x / 2
                            - cell.widget->size.x / 2;
                    cell.widget->position.y = cell.position.y + cell.widget->padd_y.y;
                    break;
                case E:
                    cell.widget->position.x = cell.position.x + cell.size.x
                            - cell.widget->size.x - cell.widget->padd_x.y;
                    cell.widget->position.y = cell.position.y + cell.size.y / 2
                            - cell.widget->size.y / 2;
                    break;
                case W:
                    cell.widget->position.x = cell.position.x + cell.widget->padd_x.x;
                    cell.widget->position.y = cell.position.y + cell.size.y / 2
                            - cell.widget->size.y / 2;
                    break;
                case NE:
                    cell.widget->position.x = cell.position.x + cell.size.x
                            - cell.widget->size.x - cell.widget->padd_x.y;
                    cell.widget->position.y = cell.position.y + cell.size.y
                            - cell.widget->size.y - cell.widget->padd_y.x;
                    break;
                case NW:
                    cell.widget->position.x = cell.position.x + cell.widget->padd_x.x;
                    cell.widget->position.y = cell.position.y + cell.size.y
                            - cell.widget->size.y - cell.widget->padd_y.x;
                    break;
                case SE:
                    cell.widget->position.x = cell.position.x + cell.widget->padd_x.x;
                    cell.widget->position.y = cell.position.y + cell.widget->padd_y.y;
                    break;
                case SW:
                    cell.widget->position.x = cell.position.x + cell.widget->padd_x.x;
                    cell.widget->position.y = cell.position.y + cell.widget->padd_y.y;
                    break;
            }
        }

        // for (Cell& cell : cells) {  // TODO remove
        //     glm::mat4 matrix = glm::mat4(1.0f);
        //     matrix = glm::translate(matrix, glm::vec3(cell.position, 0.0f));
        //     matrix = glm::scale(matrix, glm::vec3(cell.size.x, cell.size.y, 1.0f));

        //     app->gui_renderer->storage.quad2d_shader->bind();
        //     app->gui_renderer->storage.quad2d_shader->set_uniform_mat4("u_model_matrix", matrix);

        //     app->gui_renderer->FrameTextureTest->bind(0);
        //     app->gui_renderer->storage.quad2d_vertex_array->bind();

        //     glDrawArrays(GL_TRIANGLES, 0, 6);
        // }

        // Render the children
        for (std::shared_ptr<Widget> widget : children) {
            widget->render();
        }
    }

    void Frame::on_window_resized(events::WindowResizedEvent& event) {
        if (base) {
            size.x = event.width;
            size.y = event.height;
            return;
        }


    }

    void Frame::add(std::shared_ptr<Widget> widget, unsigned int row, unsigned int column,
            unsigned int row_span, unsigned int column_span, const glm::ivec2& padd_x,
            const glm::ivec2& padd_y, Sticky sticky) {
        widget->set(row, column)->span(row_span, column_span)->padd(padd_x, padd_y)->stick(sticky);
        children.push_back(widget);
    }

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

    Dummy::Dummy(std::shared_ptr<Frame> parent)
        : Widget(parent) {

    }

    void Dummy::render() {}  // Do nothing
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

    // FrameTextureTest = Texture::create("data/frame.png", false);

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

void GuiRenderer::on_window_resized(events::WindowResizedEvent& event) {
    storage.orthographic_projection_matrix = glm::ortho(
        0.0f, static_cast<float>(event.width),
        0.0f, static_cast<float>(event.height)
    );

    storage.quad2d_shader->bind();  // TODO optimize maybe
    storage.quad2d_shader->set_uniform_mat4("u_projection_matrix", storage.orthographic_projection_matrix);

    storage.text_shader->bind();
    storage.text_shader->set_uniform_mat4("u_projection_matrix", storage.orthographic_projection_matrix);

    main_frame->on_window_resized(event);
}
