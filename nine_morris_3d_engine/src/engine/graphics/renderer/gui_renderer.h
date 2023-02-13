#pragma once

#include <glm/glm.hpp>

#include "engine/application/event.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/texture.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/gui.h"
#include "engine/graphics/font.h"
#include "engine/other/encrypt.h"
#include "engine/scene/scene_list.h"

class Application;

class GuiRenderer {
public:
    struct Storage;

    GuiRenderer(Application* app);
    ~GuiRenderer();

    GuiRenderer(const GuiRenderer&) = delete;
    GuiRenderer& operator=(const GuiRenderer&) = delete;
    GuiRenderer(GuiRenderer&&) = delete;
    GuiRenderer& operator=(GuiRenderer&&) = delete;

    void render(const SceneList& scene);

    void quad_center(float& width, float& height, float& x_pos, float& y_pos);

    const Storage& get_storage() { return storage; }

    struct QuadVertex {
        glm::vec2 position;
        glm::vec2 texture_coordinate;
        float texture_index;
    };
private:
    using BeginEnd = std::function<void()>;

    void begin_quads_batch();
    void end_quads_batch();
    void flush_quads();
    void draw_quad(glm::vec2 position, glm::vec2 size, std::shared_ptr<gl::Texture> texture);

    void begin_draw_image();
    void begin_draw_text();
    void end_draw_image();
    void end_draw_text();

    void draw(const std::vector<gui::Widget*>& subwidgets, const BeginEnd& begin, const BeginEnd& end);
    bool on_window_resized(event::WindowResizedEvent& event);

    void initialize_uniform_buffers();
    void initialize_quad_renderer();
    std::shared_ptr<gl::IndexBuffer> initialize_quads_index_buffer();
    void initialize_text_renderer();
    void initialize_projection_uniform_buffer();

    struct Storage {
        std::shared_ptr<gl::UniformBuffer> projection_uniform_buffer;

        gl::UniformBlockSpecification projection_uniform_block;

        std::shared_ptr<gl::Shader> quad2d_shader;
        std::shared_ptr<gl::Shader> text_shader;

        std::shared_ptr<gl::VertexArray> quad2d_vertex_array;
        std::shared_ptr<gl::VertexBuffer> quad2d_buffer;
        std::shared_ptr<gl::IndexBuffer> quad2d_index_buffer;

        glm::mat4 orthographic_projection_matrix = glm::mat4(1.0f);

        struct {
            QuadVertex* buffer = nullptr;
            QuadVertex* buffer_pointer = nullptr;

            std::array<GLuint, 8> texture_slots;
            size_t texture_slot_index = 0;

            size_t quad_count = 0;
        } quads;
    } storage;

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
