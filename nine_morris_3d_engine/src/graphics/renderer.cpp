#include "engine/graphics/renderer.hpp"

#include <cstddef>
#include <array>
#include <algorithm>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/application_base/logging.hpp"
#include "engine/application_base/error.hpp"
#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/vertex_buffer_layout.hpp"
#include "engine/graphics/opengl/opengl.hpp"
#include "engine/graphics/material.hpp"
#include "engine/graphics/font.hpp"
#include "engine/other/utilities.hpp"

using namespace resmanager::literals;

namespace sm {
    static constexpr unsigned int PROJECTON_VIEW_UNIFORM_BLOCK_BINDING {0};
    static constexpr unsigned int DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING {1};
    static constexpr unsigned int VIEW_POSITION_BLOCK_BINDING {2};
    static constexpr unsigned int POINT_LIGHT_BLOCK_BINDING {3};
    static constexpr unsigned int LIGHT_SPACE_BLOCK_BINDING {4};
    static constexpr std::size_t SHADER_POINT_LIGHTS {4};
    static constexpr int SHADOW_MAP_UNIT {1};

    Renderer::Renderer(int width, int height, int samples, const FileSystem& fs) {
        OpenGl::initialize_default();
        OpenGl::enable_depth_test();
        OpenGl::clear_color(0.0f, 0.0f, 0.0f);

        // Scene
        {
            FramebufferSpecification specification;
            specification.width = width;
            specification.height = height;
            specification.color_attachments = {
                Attachment(AttachmentFormat::Rgba8, AttachmentType::Renderbuffer)
            };
            specification.depth_attachment = Attachment(
                AttachmentFormat::Depth32, AttachmentType::Renderbuffer
            );
            specification.samples = samples;

            storage.scene_framebuffer = std::make_shared<GlFramebuffer>(specification);

            register_framebuffer(storage.scene_framebuffer);
        }

        // Intermediate
        {
            FramebufferSpecification specification;
            specification.width = width;
            specification.height = height;
            specification.color_attachments = {
                Attachment(AttachmentFormat::Rgba8, AttachmentType::Texture)
            };
            specification.depth_attachment = Attachment(
                AttachmentFormat::Depth32, AttachmentType::Renderbuffer
            );

            storage.intermediate_framebuffer = std::make_shared<GlFramebuffer>(specification);

            register_framebuffer(storage.intermediate_framebuffer);
        }

        // Shadow
        {
            FramebufferSpecification specification;
            specification.width = 2048;
            specification.height = 2048;
            specification.depth_attachment = Attachment(AttachmentFormat::Depth32, AttachmentType::Texture);
            specification.white_border_for_depth_texture = true;
            specification.resizable = false;

            storage.shadow_map_framebuffer = std::make_shared<GlFramebuffer>(specification);

            register_framebuffer(storage.shadow_map_framebuffer);
        }

        {
            // Doesn't have uniform buffers for sure
            storage.screen_quad_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/screen_quad.vert")),
                utils::read_file(fs.path_engine_assets("shaders/screen_quad.frag"))
            );
        }

        {
            storage.shadow_shader = std::make_shared<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/shadow.vert")),
                utils::read_file(fs.path_engine_assets("shaders/shadow.frag"))
            );

            register_shader(storage.shadow_shader);
        }

        {
            // Doesn't have uniform buffers for sure
            storage.text_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/text.vert")),
                utils::read_file(fs.path_engine_assets("shaders/text.frag"))
            );
        }

        {
            // Doesn't have uniform buffers for sure
            storage.skybox_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/skybox.vert")),
                utils::read_file(fs.path_engine_assets("shaders/skybox.frag"))
            );
        }

        {
            const float vertices[] {
                -1.0f,  1.0f,
                -1.0f, -1.0f,
                 1.0f,  1.0f,
                 1.0f,  1.0f,
                -1.0f, -1.0f,
                 1.0f, -1.0f
            };

            auto vertex_buffer {std::make_shared<GlVertexBuffer>(vertices, sizeof(vertices))};

            storage.screen_quad_vertex_array = std::make_unique<GlVertexArray>();
            storage.screen_quad_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 2);

                va->add_vertex_buffer(vertex_buffer, layout);
            });
        }

        {
            auto vertex_buffer {std::make_shared<GlVertexBuffer>(CUBEMAP_VERTICES, sizeof(CUBEMAP_VERTICES))};

            storage.skybox_vertex_array = std::make_unique<GlVertexArray>();
            storage.skybox_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 3);

                va->add_vertex_buffer(vertex_buffer, layout);
            });
        }

        {
            auto vertex_buffer {std::make_shared<GlVertexBuffer>(DrawHint::Stream)};

            storage.text_vertex_array = std::make_unique<GlVertexArray>();
            storage.text_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 2);
                layout.add(1, VertexBufferLayout::Float, 2);
                layout.add(2, VertexBufferLayout::Int, 1);

                va->add_vertex_buffer(vertex_buffer, layout);
            });

            storage.wtext_vertex_buffer = vertex_buffer;
        }

        {
            storage.default_font = std::make_unique<Font>(
                utils::read_file(fs.path_engine_assets("fonts/CodeNewRoman/code-new-roman.regular.ttf")),
                FontSpecification()  // FIXME bitmap doesn't take padding into consideration
            );

            storage.default_font->begin_baking();
            storage.default_font->bake_ascii();
            storage.default_font->end_baking("default");
        }

        debug_initialize(fs);
    }

    void Renderer::capture(const Camera& camera, const glm::vec3& position) {
        scene_list.camera.view_matrix = camera.view_matrix;
        scene_list.camera.projection_matrix = camera.projection_matrix;
        scene_list.camera.projection_view_matrix = camera.projection_view_matrix;
        scene_list.camera.position = position;
    }

    void Renderer::capture(const Camera2D& camera_2d) {
        scene_list.camera_2d.projection_matrix = camera_2d.projection_matrix;
    }

    void Renderer::skybox(std::shared_ptr<GlTextureCubemap> texture) {
        storage.skybox_texture = texture;
    }

    void Renderer::shadows(float left, float right, float bottom, float top, float lens_near, float lens_far, const glm::vec3& position) {
        scene_list.light_space.left = left;
        scene_list.light_space.right = right;
        scene_list.light_space.bottom = bottom;
        scene_list.light_space.top = top;
        scene_list.light_space.lens_near = lens_near;
        scene_list.light_space.lens_far = lens_far;
        scene_list.light_space.position = position;
    }

    void Renderer::register_shader(std::shared_ptr<GlShader> shader) {
        scene_data.shaders.push_back(shader);
    }

    void Renderer::register_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer) {
        scene_data.framebuffers.push_back(framebuffer);
    }

    void Renderer::add_renderable(const Renderable& renderable) {
        scene_list.renderables.push_back(renderable);
    }

    void Renderer::add_light(const DirectionalLight& light) {
        scene_list.directional_light = light;
    }

    void Renderer::add_light(const PointLight& light) {
        scene_list.point_lights.push_back(light);
    }

    void Renderer::add_text(const Text& text) {
        scene_list.texts.push_back(text);
    }

    void Renderer::add_info_text(float fps) {
        Text text;
        text.font = storage.default_font;
        text.text = std::to_string(fps) + " FPS";
        text.position = glm::vec2(1.0f);
        text.color = glm::vec3(0.75f);

        scene_list.texts.push_back(text);
    }

    void Renderer::debug_add_line(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& color) {
        Line line;
        line.position1 = position1;
        line.position2 = position2;
        line.color = color;

        debug_scene_list.lines.push_back(line);
    }

    void Renderer::debug_add_lines(const std::vector<glm::vec3>& points, const glm::vec3& color) {
        assert(points.size() >= 2);

        Line line;
        line.color = color;

        for (std::size_t i {1}; i < points.size(); i++) {
            line.position1 = points[i - 1];
            line.position2 = points[i];

            debug_scene_list.lines.push_back(line);
        }
    }

    void Renderer::debug_add_lines(std::initializer_list<glm::vec3> points, const glm::vec3& color) {
        assert(points.size() >= 2);

        Line line;
        line.color = color;

        for (std::size_t i {1}; i < points.size(); i++) {
            line.position1 = points.begin()[i - 1];
            line.position2 = points.begin()[i];

            debug_scene_list.lines.push_back(line);
        }
    }

    void Renderer::debug_add_point(const glm::vec3& position, const glm::vec3& color) {
        static constexpr float SIZE {0.5f};

        debug_add_line(glm::vec3(-SIZE, 0.0f, 0.0f) + position, glm::vec3(SIZE, 0.0f, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, -SIZE, 0.0f) + position, glm::vec3(0.0f, SIZE, 0.0f) + position, color);
        debug_add_line(glm::vec3(0.0f, 0.0f, -SIZE) + position, glm::vec3(0.0f, 0.0f, SIZE) + position, color);
    }

    void Renderer::debug_add_lamp(const glm::vec3& position, const glm::vec3& color) {
        static constexpr float SIZE {0.3f};
        static constexpr float SIZE2 {0.15f};
        static constexpr float SIZE3 {0.5f};
        static constexpr float OFFSET {-(SIZE + SIZE3)};
        const std::array<Line, 24> LINES {
            // Top
            Line {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(SIZE, -SIZE, -SIZE), color},
            Line {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(SIZE, SIZE, SIZE), color},
            Line {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(-SIZE, -SIZE, SIZE), color},

            Line {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(-SIZE, -SIZE, SIZE), color},
            Line {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(-SIZE, SIZE, -SIZE), color},
            Line {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(SIZE, -SIZE, -SIZE), color},

            Line {glm::vec3(SIZE, -SIZE, -SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            Line {glm::vec3(-SIZE, -SIZE, SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},

            Line {glm::vec3(SIZE, SIZE, SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            Line {glm::vec3(SIZE, SIZE, SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},
            Line {glm::vec3(-SIZE, SIZE, -SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            Line {glm::vec3(-SIZE, SIZE, -SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},

            // Bottom
            Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), color},
            Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), color},
            Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), color},

            Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), color},
            Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), color},

            Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},

            Line {glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            Line {glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},
            Line {glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            Line {glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},
        };

        for (const Line& line : LINES) {
            debug_add_line(line.position1 + position, line.position2 + position, line.color);
        }
    }

    void Renderer::render() {
        // TODO pre-render setup

        {
            auto uniform_buffer {storage.wprojection_view_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                uniform_buffer->set(&scene_list.camera.projection_view_matrix, "u_projection_view_matrix"_H);
            }
        }
        {
            auto uniform_buffer {storage.wdirectional_light_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                uniform_buffer->set(&scene_list.directional_light.direction, "u_directional_light.direction"_H);
                uniform_buffer->set(&scene_list.directional_light.ambient_color, "u_directional_light.ambient"_H);
                uniform_buffer->set(&scene_list.directional_light.diffuse_color, "u_directional_light.diffuse"_H);
                uniform_buffer->set(&scene_list.directional_light.specular_color, "u_directional_light.specular"_H);
            }
        }
        {
            auto uniform_buffer {storage.wview_position_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                uniform_buffer->set(&scene_list.camera.position, "u_view_position"_H);
            }
        }
        {
            auto uniform_buffer {storage.wpoint_light_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                setup_point_light_uniform_buffer(uniform_buffer);
            }
        }
        {
            auto uniform_buffer {storage.wlight_space_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                setup_light_space_uniform_buffer(uniform_buffer);
            }
        }

        for (const auto& [_, wuniform_buffer] : storage.uniform_buffers) {
            std::shared_ptr<GlUniformBuffer> uniform_buffer {wuniform_buffer.lock()};

            if (uniform_buffer == nullptr) {
                continue;
            }

            uniform_buffer->bind();
            uniform_buffer->upload();
        }

        GlUniformBuffer::unbind();

        // Draw to depth buffer for shadows
        storage.shadow_map_framebuffer->bind();

        OpenGl::clear(OpenGl::Buffers::D);
        OpenGl::viewport(
            storage.shadow_map_framebuffer->get_specification().width,
            storage.shadow_map_framebuffer->get_specification().height
        );

        draw_renderables_to_depth_buffer();

        // Draw normal things
        storage.scene_framebuffer->bind();

        OpenGl::clear(OpenGl::Buffers::CD);
        OpenGl::viewport(
            storage.scene_framebuffer->get_specification().width,
            storage.scene_framebuffer->get_specification().height
        );

        OpenGl::bind_texture_2d(storage.shadow_map_framebuffer->get_depth_attachment(), SHADOW_MAP_UNIT);

        draw_renderables();
        draw_renderables_outlined();

        // Skybox
        if (storage.skybox_texture != nullptr) {
            draw_skybox();
        }

        // Blit the resulted scene texture to an intermediate texture, resolving anti-aliasing
        storage.scene_framebuffer->blit(
            storage.intermediate_framebuffer.get(),
            storage.scene_framebuffer->get_specification().width,
            storage.scene_framebuffer->get_specification().height
        );

        // Do post processing and render the final 3D image to the screen
        storage.intermediate_framebuffer->bind();

        OpenGl::viewport(
            storage.intermediate_framebuffer->get_specification().width,
            storage.intermediate_framebuffer->get_specification().height
        );

        end_rendering();

        // 2D stuff
        draw_texts();

        // Debug stuff
        debug_render();

        // Purge
        clear();
        debug_clear();
    }

    void Renderer::pre_setup() {
        for (const std::weak_ptr<GlShader>& wshader : scene_data.shaders) {
            std::shared_ptr<GlShader> shader {wshader.lock()};

            if (shader == nullptr) {
                continue;
            }

            // Create and store references to particular uniform buffers
            for (const UniformBlockSpecification& block : shader->uniform_blocks) {
                // Don't create duplicate buffers
                if (storage.uniform_buffers.find(block.binding_index) != storage.uniform_buffers.cend()) {
                    continue;
                }

                auto uniform_buffer {std::make_shared<GlUniformBuffer>(block)};
                shader->add_uniform_buffer(uniform_buffer);

                storage.uniform_buffers[block.binding_index] = uniform_buffer;

                switch (block.binding_index) {
                    case PROJECTON_VIEW_UNIFORM_BLOCK_BINDING:
                        storage.wprojection_view_uniform_buffer = uniform_buffer;
                        break;
                    case DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING:
                        storage.wdirectional_light_uniform_buffer = uniform_buffer;
                        break;
                    case VIEW_POSITION_BLOCK_BINDING:
                        storage.wview_position_uniform_buffer = uniform_buffer;
                        break;
                    case POINT_LIGHT_BLOCK_BINDING:
                        storage.wpoint_light_uniform_buffer = uniform_buffer;
                        break;
                    case LIGHT_SPACE_BLOCK_BINDING:
                        storage.wlight_space_uniform_buffer = uniform_buffer;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void Renderer::post_setup() {
        scene_data.shaders.clear();
    }

    void Renderer::clear() {
        scene_list.renderables.clear();
        scene_list.directional_light = {};
        scene_list.point_lights.clear();
        scene_list.texts.clear();

        storage.text_batches.clear();
        storage.texts_buffer.clear();
    }

    void Renderer::resize_framebuffers(int width, int height) {
        if (width == 0 || height == 0) {
            return;
        }

        OpenGl::viewport(width, height);

        for (std::weak_ptr<GlFramebuffer> wframebuffer : scene_data.framebuffers) {
            std::shared_ptr<GlFramebuffer> framebuffer {wframebuffer.lock()};

            if (framebuffer == nullptr) {
                continue;
            }

            if (!framebuffer->get_specification().resizable) {
                continue;
            }

            framebuffer->resize(width, height);
        }
    }

    void Renderer::screen_quad(unsigned int texture) {
        storage.screen_quad_shader->bind();
        OpenGl::bind_texture_2d(texture, 0);
        OpenGl::draw_arrays(6);
    }

    void Renderer::post_processing() {

    }

    void Renderer::end_rendering() {
        storage.screen_quad_vertex_array->bind();

        OpenGl::disable_depth_test();

        post_processing();

        // Draw the final result to the screen
        GlFramebuffer::bind_default();

        // Clear even the default framebuffer, for debug renderer
        OpenGl::clear(OpenGl::Buffers::CD);

        // screen_quad(post_processing_context.last_texture);  // FIXME
        screen_quad(storage.intermediate_framebuffer->get_color_attachment(0));

        OpenGl::enable_depth_test();

        GlVertexArray::unbind();
    }

    void Renderer::draw_renderables() {
        for (const Renderable& renderable : scene_list.renderables) {
            auto material {renderable.material.lock()};

            if (material->flags & Material::Outline) {
                continue;  // This one is rendered differently
            }

            if (material->flags & Material::DisableBackFaceCulling) {  // FIXME improve; maybe use scene graph
                OpenGl::disable_back_face_culling();
                draw_renderable(renderable);
                OpenGl::enable_back_face_culling();
            } else {
                draw_renderable(renderable);
            }
        }

        GlVertexArray::unbind();  // Don't unbind for every renderable
    }

    void Renderer::draw_renderable(const Renderable& renderable) {
        auto vertex_array {renderable.vertex_array.lock()};
        auto material {renderable.material.lock()};

        const glm::mat4 matrix {get_renderable_transform(renderable)};

        vertex_array->bind();
        material->bind_and_upload();  // TODO sort and batch models based on material
        material->get_shader()->upload_uniform_mat4("u_model_matrix"_H, matrix);

        OpenGl::draw_elements(vertex_array->get_index_buffer()->get_index_count());
    }

    void Renderer::draw_renderables_outlined() {

    }

    void Renderer::draw_renderable_outlined(const Renderable& renderable) {

    }

    void Renderer::draw_renderables_to_depth_buffer() {
        storage.shadow_shader->bind();

        for (const Renderable& renderable : scene_list.renderables) {
            auto vertex_array {renderable.vertex_array.lock()};
            auto material {renderable.material.lock()};

            if (!(material->flags & Material::CastShadow)) {
                continue;
            }

            const glm::mat4 matrix {get_renderable_transform(renderable)};

            vertex_array->bind();
            storage.shadow_shader->upload_uniform_mat4("u_model_matrix"_H, matrix);

            OpenGl::draw_elements(vertex_array->get_index_buffer()->get_index_count());
        }

        // Don't unbind for every renderable
        GlVertexArray::unbind();
    }

    void Renderer::draw_skybox() {
        const glm::mat4& projection {scene_list.camera.projection_matrix};
        const glm::mat4 view {glm::mat4(glm::mat3(scene_list.camera.view_matrix))};

        storage.skybox_shader->bind();
        storage.skybox_shader->upload_uniform_mat4("u_projection_view_matrix"_H, projection * view);

        storage.skybox_vertex_array->bind();
        storage.skybox_texture->bind(0);

        OpenGl::draw_arrays(36);

        GlVertexArray::unbind();
    }

    void Renderer::draw_texts() {
        storage.text_shader->bind();

        OpenGl::disable_depth_test();

        std::stable_sort(scene_list.texts.begin(), scene_list.texts.end(), [](const Text& lhs, const Text& rhs) {
            return lhs.font.lock().get() < rhs.font.lock().get();
        });

        const void* font_ptr {nullptr};  // TODO C++20

        for (const auto& text : scene_list.texts) {
            const void* this_ptr {text.font.lock().get()};

            assert(this_ptr != nullptr);

            if (this_ptr != font_ptr) {
                font_ptr = this_ptr;

                storage.text_batches.emplace_back().font = text.font;
            }

            if (storage.text_batches.back().objects.size() > 10) {
                storage.text_batches.emplace_back().font = text.font;
            }

            storage.text_batches.back().objects.push_back(text);
        }

        for (const auto& batch : storage.text_batches) {
            draw_text_batch(batch);
        }

        OpenGl::enable_depth_test();

        GlVertexArray::unbind();
    }

    void Renderer::draw_text_batch(const TextBatch& batch) {
        auto font {batch.font.lock()};

        std::size_t i {};  // TODO C++20

        for (const Text& text : batch.objects) {
            assert(i < 10);

            // Pushes the rendered text onto the buffer
            font->render(text.text, static_cast<int>(i), storage.texts_buffer);

            glm::mat4 matrix {1.0f};
            matrix = glm::translate(matrix, glm::vec3(text.position, 0.0f));
            matrix = glm::scale(matrix, glm::vec3(text.scale, text.scale, 1.0f));

            const std::string index {std::to_string(i)};

            storage.text_shader->upload_uniform_mat4(resmanager::HashedStr64("u_model_matrix[" + index + ']'), matrix);
            storage.text_shader->upload_uniform_vec3(resmanager::HashedStr64("u_color[" + index + ']'), text.color);

            const float border_width {text.shadows ? 0.3f : 0.0f};
            const float offset {text.shadows ? -0.003f : 0.0f};

            storage.text_shader->upload_uniform_float(resmanager::HashedStr64("u_border_width[" + index + ']'), border_width);
            storage.text_shader->upload_uniform_vec2(resmanager::HashedStr64("u_offset[" + index + ']'), glm::vec2(offset, offset));

            i++;
        }

        storage.text_shader->upload_uniform_mat4("u_projection_matrix"_H, scene_list.camera_2d.projection_matrix);

        auto vertex_buffer {storage.wtext_vertex_buffer.lock()};
        vertex_buffer->bind();
        vertex_buffer->upload_data(storage.texts_buffer.data(), storage.texts_buffer.size());
        GlVertexBuffer::unbind();

        static constexpr std::size_t ITEMS_PER_VERTEX {5};
        static constexpr std::size_t ITEM_SIZE {4};

        assert(storage.texts_buffer.size() % (ITEM_SIZE * ITEMS_PER_VERTEX) == 0);

        const int vertex_count {static_cast<int>(storage.texts_buffer.size() / (ITEM_SIZE * ITEMS_PER_VERTEX))};

        storage.text_vertex_array->bind();

        OpenGl::bind_texture_2d(font->get_bitmap()->get_id(), 0);

        OpenGl::draw_arrays(vertex_count);
    }

    void Renderer::setup_point_light_uniform_buffer(const std::shared_ptr<GlUniformBuffer> uniform_buffer) {
        // Sort front to back with respect to the camera; lights in the front of the list will be used
        std::sort(
            scene_list.point_lights.begin(),
            scene_list.point_lights.end(),
            [this](const PointLight& lhs, const PointLight& rhs) {
                const float distance_left {glm::distance(lhs.position, scene_list.camera.position)};
                const float distance_right {glm::distance(rhs.position, scene_list.camera.position)};

                return distance_left < distance_right;
            }
        );

        // Add dummy point lights to make the size 4, which is a requirement from the shader
        if (scene_list.point_lights.size() < SHADER_POINT_LIGHTS) {
            scene_list.point_lights.resize(SHADER_POINT_LIGHTS);
        }

        for (std::size_t i {0}; i < SHADER_POINT_LIGHTS; i++) {
            const PointLight& light {scene_list.point_lights[i]};
            const std::string index {std::to_string(i)};

            uniform_buffer->set(&light.position, resmanager::HashedStr64("u_point_lights[" + index + "].position"));
            uniform_buffer->set(&light.ambient_color, resmanager::HashedStr64("u_point_lights[" + index + "].ambient"));
            uniform_buffer->set(&light.diffuse_color, resmanager::HashedStr64("u_point_lights[" + index + "].diffuse"));
            uniform_buffer->set(&light.specular_color, resmanager::HashedStr64("u_point_lights[" + index + "].specular"));
            uniform_buffer->set(&light.falloff_linear, resmanager::HashedStr64("u_point_lights[" + index + "].falloff_linear"));
            uniform_buffer->set(&light.falloff_quadratic, resmanager::HashedStr64("u_point_lights[" + index + "].falloff_quadratic"));
        }
    }

    void Renderer::setup_light_space_uniform_buffer(std::shared_ptr<GlUniformBuffer> uniform_buffer) {
        const glm::mat4 projection {
            glm::ortho(
                scene_list.light_space.left,
                scene_list.light_space.right,
                scene_list.light_space.bottom,
                scene_list.light_space.top,
                scene_list.light_space.lens_near,
                scene_list.light_space.lens_far
            )
        };

        const glm::mat4 view {
            glm::lookAt(
                scene_list.light_space.position,
                scene_list.directional_light.direction,
                glm::vec3(0.0f, 1.0f, 0.0f)
            )
        };

        const glm::mat4 light_space_matrix {projection * view};

        uniform_buffer->set(&light_space_matrix, "u_light_space_matrix"_H);
    }

    glm::mat4 Renderer::get_renderable_transform(const Renderable& renderable) {
        glm::mat4 matrix {1.0f};

        if (renderable.transform.index() == 0) {
            matrix = glm::translate(matrix, std::get<0>(renderable.transform).position);
            matrix = glm::rotate(matrix, std::get<0>(renderable.transform).rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::rotate(matrix, std::get<0>(renderable.transform).rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, std::get<0>(renderable.transform).rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::scale(matrix, glm::vec3(std::get<0>(renderable.transform).scale));
        } else {
            matrix = std::get<1>(renderable.transform);
        }

        return matrix;
    }

    void Renderer::debug_initialize(const FileSystem& fs) {
        debug_storage.shader = std::make_shared<GlShader>(
            utils::read_file(fs.path_engine_assets("shaders/debug.vert")),
            utils::read_file(fs.path_engine_assets("shaders/debug.frag"))
        );

        register_shader(debug_storage.shader);

        auto vertex_buffer {std::make_shared<GlVertexBuffer>(DrawHint::Stream)};
        debug_storage.wvertex_buffer = vertex_buffer;

        debug_storage.vertex_array = std::make_unique<GlVertexArray>();
        debug_storage.vertex_array->configure([&](GlVertexArray* va) {
            VertexBufferLayout layout;
            layout.add(0, VertexBufferLayout::Float, 3);
            layout.add(1, VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
        });
    }

    void Renderer::debug_render() {
        for (const Line& line : debug_scene_list.lines) {
            BufferVertex v1;
            v1.position = line.position1;
            v1.color = line.color;

            debug_storage.lines_buffer.push_back(v1);

            BufferVertex v2;
            v2.position = line.position2;
            v2.color = line.color;

            debug_storage.lines_buffer.push_back(v2);
        }

        if (debug_storage.lines_buffer.empty()) {
            return;
        }

        auto vertex_buffer {debug_storage.wvertex_buffer.lock()};

        vertex_buffer->bind();
        vertex_buffer->upload_data(debug_storage.lines_buffer.data(), debug_storage.lines_buffer.size() * sizeof(BufferVertex));
        GlVertexBuffer::unbind();

        debug_storage.shader->bind();
        debug_storage.vertex_array->bind();

        OpenGl::draw_arrays_lines(static_cast<int>(debug_scene_list.lines.size()) * 2);

        GlVertexArray::unbind();
    }

    void Renderer::debug_clear() {
        debug_scene_list.lines.clear();
        debug_storage.lines_buffer.clear();
    }
}
