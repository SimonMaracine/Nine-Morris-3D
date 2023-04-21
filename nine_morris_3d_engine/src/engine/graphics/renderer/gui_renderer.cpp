#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <resmanager/resmanager.h>

#include "engine/application_base/application.h"
#include "engine/application_base/events.h"
#include "engine/application_base/capabilities.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/graphics/opengl/vertex_array.h"
#include "engine/graphics/opengl/buffer.h"
#include "engine/graphics/opengl/texture.h"
#include "engine/graphics/renderer/gui_renderer.h"
#include "engine/graphics/renderer/render_helpers.h"
#include "engine/graphics/font.h"
#include "engine/graphics/vertex_buffer_layout.h"
#include "engine/graphics/gui.h"
#include "engine/other/file_system.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/scene/scene_list.h"

namespace sm {
    static constexpr size_t MAX_QUAD_COUNT = 1000;
    static constexpr size_t MAX_VERTEX_BUFFER_SIZE = sizeof(GuiRenderer::QuadVertex) * 4 * MAX_QUAD_COUNT;
    static constexpr size_t MAX_INDICES = 6 * MAX_QUAD_COUNT;

    static float map(float x, float in_min, float in_max, float out_min, float out_max) {  // TODO maybe put this in utilities
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    GuiRenderer::GuiRenderer(Ctx* ctx)
        : ctx(ctx) {
        storage.orthographic_projection_matrix = glm::ortho(
            0.0f, static_cast<float>(ctx->properties->width),
            0.0f, static_cast<float>(ctx->properties->height)
        );

        initialize_uniform_buffers();
        initialize_quad_renderer();
        initialize_text_renderer();
        initialize_projection_uniform_buffer();
        initialize_debug_text();

        ctx->evt.connect<WindowResizedEvent, &GuiRenderer::on_window_resized>(this);

        // Set application pointer to widgets
        gui::Widget::ctx = ctx;

        LOG_INFO("Initialized GUI renderer");
    }

    GuiRenderer::~GuiRenderer() {
        delete[] storage.quads.buffer;

        LOG_INFO("Uninitialized GUI renderer");
    }

    void GuiRenderer::render(const SceneList& scene, bool debug_text) {
        static std::vector<gui::Widget*> images;
        static std::vector<gui::Widget*> texts;

        images.clear();
        texts.clear();

        std::for_each(scene.images.cbegin(), scene.images.cend(), [&](gui::Image* image) {
            images.push_back(image);
        });

        std::for_each(scene.texts.cbegin(), scene.texts.cend(), [&](gui::Text* text) {
            texts.push_back(text);
        });

        render_helpers::disable_depth_test();

        draw(images, std::bind(&GuiRenderer::begin_draw_image, this), std::bind(&GuiRenderer::end_draw_image, this));  // TODO use something else than std::bind
        draw(texts, std::bind(&GuiRenderer::begin_draw_text, this), std::bind(&GuiRenderer::end_draw_text, this));

        if (debug_text) {
            render_debug_text();  // FIXME implement this
        }

        render_helpers::enable_depth_test();
    }

    void GuiRenderer::quad_center(float& width, float& height, float& x_pos, float& y_pos) {
        if (static_cast<float>(ctx->properties->width) / ctx->properties->height > 16.0f / 9.0f) {
            width = ctx->properties->width;
            height = ctx->properties->width * (9.0f / 16.0f);
            x_pos = 0.0f;
            y_pos = (height - ctx->properties->height) / -2.0f;
        } else {
            height = ctx->properties->height;
            width = ctx->properties->height * (16.0f / 9.0f);
            x_pos = (width - ctx->properties->width) / -2.0f;
            y_pos = 0.0f;
        }
    }

    void GuiRenderer::begin_quads_batch() {
        storage.quads.quad_count = 0;
        storage.quads.buffer_pointer = storage.quads.buffer;
        storage.quads.texture_slot_index = 0;
    }

    void GuiRenderer::end_quads_batch() {
        const size_t size = sizeof(QuadVertex) * (storage.quads.buffer_pointer - storage.quads.buffer);

        storage.quad2d_buffer->bind();
        storage.quad2d_buffer->upload_sub_data(storage.quads.buffer, 0, size);
    }

    void GuiRenderer::flush_quads() {
        storage.quad2d_vertex_array->bind();
        storage.quad2d_shader->bind();

        for (size_t i = 0; i < storage.quads.texture_slot_index; i++) {
            render_helpers::bind_texture_2d(storage.quads.texture_slots[i], i);
        }

        render_helpers::draw_elements(storage.quads.quad_count * 6);
    }

    void GuiRenderer::draw_quad(glm::vec2 position, glm::vec2 size, std::shared_ptr<gl::Texture> texture) {
        if (storage.quads.quad_count == MAX_QUAD_COUNT || storage.quads.texture_slot_index == storage.quads.MAX_TEXTURE_UNITS) {
            end_quads_batch();
            flush_quads();
            begin_quads_batch();
        }

        float texture_index = -1.0f;

        // Search for this texture in slots array
        for (size_t i = 0; i < storage.quads.texture_slot_index; i++) {
            if (storage.quads.texture_slots[i] == texture->get_id()) {
                texture_index = static_cast<float>(i);
                break;
            }
        }

        if (texture_index == -1.0f) {
            // Not found in slots
            texture_index = static_cast<float>(storage.quads.texture_slot_index);
            storage.quads.texture_slots[storage.quads.texture_slot_index] = texture->get_id();
            storage.quads.texture_slot_index++;
        }

        storage.quads.buffer_pointer->position = glm::vec2(position.x + size.x, position.y + size.y);
        storage.quads.buffer_pointer->texture_coordinate = glm::vec2(1.0f, 1.0f);
        storage.quads.buffer_pointer->texture_index = texture_index;
        storage.quads.buffer_pointer++;

        storage.quads.buffer_pointer->position = glm::vec2(position.x, position.y + size.y);
        storage.quads.buffer_pointer->texture_coordinate = glm::vec2(0.0f, 1.0f);
        storage.quads.buffer_pointer->texture_index = texture_index;
        storage.quads.buffer_pointer++;

        storage.quads.buffer_pointer->position = glm::vec2(position.x, position.y);
        storage.quads.buffer_pointer->texture_coordinate = glm::vec2(0.0f, 0.0f);
        storage.quads.buffer_pointer->texture_index = texture_index;
        storage.quads.buffer_pointer++;

        storage.quads.buffer_pointer->position = glm::vec2(position.x + size.x, position.y);
        storage.quads.buffer_pointer->texture_coordinate = glm::vec2(1.0f, 0.0f);
        storage.quads.buffer_pointer->texture_index = texture_index;
        storage.quads.buffer_pointer++;

        storage.quads.quad_count++;
    }

    void GuiRenderer::begin_draw_image() {
        begin_quads_batch();
    }

    void GuiRenderer::end_draw_image() {
        end_quads_batch();
        flush_quads();
    }

    void GuiRenderer::begin_draw_text() {
        storage.text_shader->bind();
    }

    void GuiRenderer::end_draw_text() {}

    void GuiRenderer::draw(const std::vector<gui::Widget*>& subwidgets, const BeginEnd& begin, const BeginEnd& end) {
        begin();

        for (gui::Widget* widget : subwidgets) {
            const int WINDOW_WIDTH = ctx->properties->width;
            const int WINDOW_HEIGHT = ctx->properties->height;

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

            if (widget->fake.fake) {
                widget->size = widget->fake.fake_size;
            }

            switch (widget->sticky) {
                case gui::Sticky::Center:
                    widget->position = glm::vec2(
                        WINDOW_WIDTH / 2.0f - widget->size.x * SCALE / 2.0f,
                        WINDOW_HEIGHT / 2.0f - widget->size.y * SCALE / 2.0f
                    );
                    break;
                case gui::Sticky::N:
                    widget->position = glm::vec2(
                        WINDOW_WIDTH / 2.0f - widget->size.x * SCALE / 2.0f,
                        WINDOW_HEIGHT - widget->size.y * SCALE - widget->offset_parameters.top
                    );
                    break;
                case gui::Sticky::S:
                    widget->position = glm::vec2(
                        WINDOW_WIDTH / 2.0f - widget->size.x * SCALE / 2.0f,
                        widget->offset_parameters.bottom
                    );
                    break;
                case gui::Sticky::E:
                    widget->position = glm::vec2(
                        WINDOW_WIDTH - widget->size.x * SCALE - widget->offset_parameters.right,
                        WINDOW_HEIGHT / 2.0f - widget->size.y * SCALE / 2.0f
                    );
                    break;
                case gui::Sticky::W:
                    widget->position = glm::vec2(
                        widget->offset_parameters.left,
                        WINDOW_HEIGHT / 2.0f - widget->size.y * SCALE / 2.0f
                    );
                    break;
                case gui::Sticky::NE:
                    widget->position = glm::vec2(
                        WINDOW_WIDTH - widget->size.x * SCALE - widget->offset_parameters.right,
                        WINDOW_HEIGHT - widget->size.y * SCALE - widget->offset_parameters.top
                    );
                    break;
                case gui::Sticky::NW:
                    widget->position = glm::vec2(
                        widget->offset_parameters.left,
                        WINDOW_HEIGHT - widget->size.y * SCALE - widget->offset_parameters.top
                    );
                    break;
                case gui::Sticky::SE:
                    widget->position = glm::vec2(
                        WINDOW_WIDTH - widget->size.x * SCALE - widget->offset_parameters.right,
                        widget->offset_parameters.bottom
                    );
                    break;
                case gui::Sticky::SW:
                    widget->position = glm::vec2(
                        widget->offset_parameters.left,
                        widget->offset_parameters.bottom
                    );
                    break;
            }

            widget->render();
        }

        end();

        // Vertex arrays that were bound in widget->render()
        gl::VertexArray::unbind();
    }

    void GuiRenderer::on_window_resized(const WindowResizedEvent& event) {
        storage.orthographic_projection_matrix = glm::ortho(
            0.0f, static_cast<float>(event.width),
            0.0f, static_cast<float>(event.height)
        );

        // Should already be configured
        storage.projection_uniform_buffer->set(&storage.orthographic_projection_matrix, 0);
        storage.projection_uniform_buffer->bind();
        storage.projection_uniform_buffer->upload_sub_data();

        gl::UniformBuffer::unbind();
    }

    void GuiRenderer::render_debug_text() {
        // FIXME implement this
    }

    void GuiRenderer::initialize_uniform_buffers() {
        storage.projection_uniform_buffer = std::make_shared<gl::UniformBuffer>();

        storage.projection_uniform_block.block_name = "Projection";
        storage.projection_uniform_block.field_names = { "u_projection_matrix" };
        storage.projection_uniform_block.uniform_buffer = storage.projection_uniform_buffer;
        storage.projection_uniform_block.binding_index = 4;
    }

    void GuiRenderer::initialize_quad_renderer() {
        storage.quad2d_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(QUAD2D_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(QUAD2D_FRAGMENT_SHADER)),
            std::vector<std::string> {
                "u_texture[0]",  // FIXME this should be max
                "u_texture[1]",
                "u_texture[2]",
                "u_texture[3]",
                "u_texture[4]",
                "u_texture[5]",
                "u_texture[6]",
                "u_texture[7]"
            },
            std::initializer_list { storage.projection_uniform_block }
        );

        storage.quad2d_shader->bind();

        for (size_t i = 0; i < 8; i++) {  // FIXME should be storage.quads.MAX_TEXTURE_UNITS
            storage.quad2d_shader->upload_uniform_int(
                resmanager::HashedStr64("u_texture[" + std::to_string(i) + "]"),
                i
            );
        }

        gl::Shader::unbind();

        storage.quad2d_buffer = std::make_shared<gl::VertexBuffer>(MAX_VERTEX_BUFFER_SIZE, gl::DrawHint::Stream);
        storage.quad2d_index_buffer = initialize_quads_index_buffer();

        VertexBufferLayout layout = VertexBufferLayout {}
            .add(0, VertexBufferLayout::Float, 2)
            .add(1, VertexBufferLayout::Float, 2)
            .add(2, VertexBufferLayout::Float, 1);

        storage.quad2d_vertex_array = std::make_shared<gl::VertexArray>();
        storage.quad2d_vertex_array->begin_definition()
            .add_buffer(storage.quad2d_buffer, layout)
            .add_index_buffer(storage.quad2d_index_buffer)
            .end_definition();

        storage.quads.buffer = new QuadVertex[MAX_QUAD_COUNT];
        storage.quads.MAX_TEXTURE_UNITS = max_texture_units_supported();
        storage.quads.texture_slots.resize(storage.quads.MAX_TEXTURE_UNITS);
    }

    std::shared_ptr<gl::IndexBuffer> GuiRenderer::initialize_quads_index_buffer() {
        unsigned int* buffer = new unsigned int[MAX_INDICES];
        size_t offset = 0;

        for (size_t i = 0; i < MAX_QUAD_COUNT * 6; i += 6) {
            buffer[i + 0] = 0 + offset;
            buffer[i + 1] = 1 + offset;
            buffer[i + 2] = 2 + offset;
            buffer[i + 3] = 2 + offset;
            buffer[i + 4] = 3 + offset;
            buffer[i + 5] = 0 + offset;

            offset += 4;
        }

        auto index_buffer = std::make_shared<gl::IndexBuffer>(buffer, sizeof(unsigned int) * MAX_INDICES);

        delete[] buffer;

        return index_buffer;
    }

    void GuiRenderer::initialize_text_renderer() {
        storage.text_shader = std::make_shared<gl::Shader>(
            Encrypt::encr(file_system::path_for_assets(TEXT_VERTEX_SHADER)),
            Encrypt::encr(file_system::path_for_assets(TEXT_FRAGMENT_SHADER)),
            std::vector<std::string> {
                "u_model_matrix",
                "u_bitmap",
                "u_color",
                "u_border_width",
                "u_offset"
            },
            std::initializer_list { storage.projection_uniform_block }
        );
    }

    void GuiRenderer::initialize_projection_uniform_buffer() {
        // Should already be configured
        storage.projection_uniform_buffer->set(&storage.orthographic_projection_matrix, 0);
        storage.projection_uniform_buffer->bind();
        storage.projection_uniform_buffer->upload_sub_data();

        gl::UniformBuffer::unbind();
    }

    void GuiRenderer::initialize_debug_text() {
        storage.debug_font = std::make_shared<Font>(DEBUG_TEXT_FONT, 14.0f, 6, 180, 40, 512);

        // FIXME implement this
    }
}
