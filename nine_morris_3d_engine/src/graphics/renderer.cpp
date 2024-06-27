#include "engine/graphics/renderer.hpp"

#include <cstddef>
#include <algorithm>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/vertex_buffer_layout.hpp"
#include "engine/graphics/opengl/opengl.hpp"
#include "engine/graphics/material.hpp"
#include "engine/graphics/font.hpp"
#include "engine/other/utilities.hpp"

// shader uniform limit https://www.khronos.org/opengl/wiki/Uniform_(GLSL)#Implementation_limits

using namespace resmanager::literals;

namespace sm {
    DebugRenderer::DebugRenderer(const FileSystem& fs, Renderer& renderer) {
        storage.shader = std::make_shared<GlShader>(
            utils::read_file(fs.path_engine_assets("shaders/debug.vert")),
            utils::read_file(fs.path_engine_assets("shaders/debug.frag"))
        );

        renderer.register_shader(storage.shader);

        const auto vertex_buffer {std::make_shared<GlVertexBuffer>(DrawHint::Stream)};
        storage.wvertex_buffer = vertex_buffer;

        storage.vertex_array = std::make_unique<GlVertexArray>();
        storage.vertex_array->configure([&](GlVertexArray* va) {
            VertexBufferLayout layout;
            layout.add(0, VertexBufferLayout::Float, 3);
            layout.add(1, VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
        });
    }

    void DebugRenderer::render() {
        for (const Line& line : scene.lines) {
            BufferVertex v1;
            v1.position = line.position1;
            v1.color = line.color;

            storage.lines_buffer.push_back(v1);

            BufferVertex v2;
            v2.position = line.position2;
            v2.color = line.color;

            storage.lines_buffer.push_back(v2);
        }

        if (storage.lines_buffer.empty()) {
            return;
        }

        const auto vertex_buffer {storage.wvertex_buffer.lock()};

        vertex_buffer->bind();
        vertex_buffer->upload_data(storage.lines_buffer.data(), storage.lines_buffer.size() * sizeof(BufferVertex));
        GlVertexBuffer::unbind();

        storage.shader->bind();
        storage.vertex_array->bind();

        opengl::draw_arrays_lines(static_cast<int>(scene.lines.size()) * 2);

        GlVertexArray::unbind();
    }

    void DebugRenderer::clear() {
        scene.lines.clear();
        storage.lines_buffer.clear();
    }

    Renderer::Renderer(int width, int height, int samples, const FileSystem& fs, const ShaderLibrary&) {
        opengl::initialize_default();
        opengl::initialize_stencil();
        opengl::enable_depth_test();
        opengl::clear_color(0.0f, 0.0f, 0.0f);

        // Scene
        {
            FramebufferSpecification specification;
            specification.width = width;
            specification.height = height;
            specification.color_attachments = {
                Attachment(AttachmentFormat::Rgba8, AttachmentType::Renderbuffer)
            };
            specification.depth_attachment = Attachment(
                AttachmentFormat::Depth24Stencil8, AttachmentType::Renderbuffer
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
            storage.quad_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/quad.vert")),
                utils::read_file(fs.path_engine_assets("shaders/quad.frag"))
            );

            storage.quad_shader->bind();
            storage.quad_shader->upload_uniform_int_array("u_texture[0]"_H, {0, 1, 2, 3, 4, 5, 6, 7});
            GlShader::unbind();
        }

        {
            // Doesn't have uniform buffers for sure
            storage.skybox_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/skybox.vert")),
                utils::read_file(fs.path_engine_assets("shaders/skybox.frag"))
            );
        }

        {
            storage.outline_shader = std::make_shared<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/outline.vert")),
                utils::read_file(fs.path_engine_assets("shaders/outline.frag"))
            );

            register_shader(storage.outline_shader);
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

            const auto vertex_buffer {std::make_shared<GlVertexBuffer>(vertices, sizeof(vertices))};

            storage.screen_quad_vertex_array = std::make_unique<GlVertexArray>();
            storage.screen_quad_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 2);

                va->add_vertex_buffer(vertex_buffer, layout);
            });
        }

        {
            const auto vertex_buffer {std::make_shared<GlVertexBuffer>(CUBEMAP_VERTICES, sizeof(CUBEMAP_VERTICES))};

            storage.skybox_vertex_array = std::make_unique<GlVertexArray>();
            storage.skybox_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 3);

                va->add_vertex_buffer(vertex_buffer, layout);
            });
        }

        {
            const auto vertex_buffer {std::make_shared<GlVertexBuffer>(DrawHint::Stream)};

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
            const auto vertex_buffer {std::make_shared<GlVertexBuffer>(MAX_QUADS_BUFFER_SIZE, DrawHint::Stream)};
            const auto index_buffer {initialize_quads_index_buffer()};

            storage.quad_vertex_array = std::make_unique<GlVertexArray>();
            storage.quad_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 2);
                layout.add(1, VertexBufferLayout::Float, 2);
                layout.add(2, VertexBufferLayout::Int, 1);

                va->add_vertex_buffer(vertex_buffer, layout);
                va->add_index_buffer(index_buffer);
            });

            storage.wquad_vertex_buffer = vertex_buffer;

            storage.quad.buffer = std::make_unique<QuadVertex[]>(MAX_QUAD_COUNT * 4);
        }

        {
            storage.default_font = std::make_unique<Font>(
                utils::read_file(fs.path_engine_assets("fonts/CodeNewRoman/code-new-roman.regular.ttf"))
            );

            storage.default_font->begin_baking();
            storage.default_font->bake_ascii();
            storage.default_font->end_baking("default");
        }

#ifndef SM_BUILD_DISTRIBUTION
        debug = DebugRenderer(fs, *this);
#endif
    }

    void Renderer::capture(const Camera& camera, const glm::vec3& position) {
        scene.camera.view_matrix = camera.view_matrix;
        scene.camera.projection_matrix = camera.projection_matrix;
        scene.camera.projection_view_matrix = camera.projection_view_matrix;
        scene.camera.position = position;
    }

    void Renderer::capture(const Camera2D& camera_2d) {
        scene.camera_2d.projection_matrix = camera_2d.projection_matrix;
    }

    void Renderer::skybox(std::shared_ptr<GlTextureCubemap> texture) {
        storage.skybox_texture = texture;
    }

    void Renderer::shadows(float left, float right, float bottom, float top, float lens_near, float lens_far, const glm::vec3& position) {
        scene.light_space.left = left;
        scene.light_space.right = right;
        scene.light_space.bottom = bottom;
        scene.light_space.top = top;
        scene.light_space.lens_near = lens_near;
        scene.light_space.lens_far = lens_far;
        scene.light_space.position = position;
    }

    void Renderer::register_shader(std::shared_ptr<GlShader> shader) {
        storage.scene.shaders.push_back(shader);
    }

    void Renderer::register_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer) {
        storage.scene.framebuffers.push_back(framebuffer);
    }

    void Renderer::add_renderable(const Renderable& renderable) {
        scene.renderables.push_back(renderable);
    }

    void Renderer::add_light(const DirectionalLight& light) {
        scene.directional_light = light;
    }

    void Renderer::add_light(const PointLight& light) {
        scene.point_lights.push_back(light);
    }

    void Renderer::add_text(const Text& text) {
        scene.texts.push_back(text);
    }

    void Renderer::add_info_text(float fps) {
        Text text;
        text.font = storage.default_font;
        text.text = std::to_string(fps) + " FPS";
        text.color = glm::vec3(1.0f);

        scene.texts.push_back(text);
    }

    void Renderer::add_quad(const Quad& quad) {
        scene.quads.push_back(quad);
    }

    void Renderer::debug_add_line(const glm::vec3& position1, const glm::vec3& position2, const glm::vec3& color) {
        DebugRenderer::Line line;
        line.position1 = position1;
        line.position2 = position2;
        line.color = color;

#ifndef SM_BUILD_DISTRIBUTION
        debug.scene.lines.push_back(line);
#endif
    }

    void Renderer::debug_add_lines(const std::vector<glm::vec3>& points, const glm::vec3& color) {
        assert(points.size() >= 2);

        DebugRenderer::Line line;
        line.color = color;

        for (std::size_t i {1}; i < points.size(); i++) {
            line.position1 = points[i - 1];
            line.position2 = points[i];

#ifndef SM_BUILD_DISTRIBUTION
            debug.scene.lines.push_back(line);
#endif
        }
    }

    void Renderer::debug_add_lines(std::initializer_list<glm::vec3> points, const glm::vec3& color) {
        assert(points.size() >= 2);

        DebugRenderer::Line line;
        line.color = color;

        for (std::size_t i {1}; i < points.size(); i++) {
            line.position1 = points.begin()[i - 1];
            line.position2 = points.begin()[i];

#ifndef SM_BUILD_DISTRIBUTION
            debug.scene.lines.push_back(line);
#endif
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
        const std::array<DebugRenderer::Line, 24> LINES {
            // Top
            DebugRenderer::Line {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(SIZE, -SIZE, -SIZE), color},
            DebugRenderer::Line {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(SIZE, SIZE, SIZE), color},
            DebugRenderer::Line {glm::vec3(SIZE, -SIZE, SIZE), glm::vec3(-SIZE, -SIZE, SIZE), color},

            DebugRenderer::Line {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(-SIZE, -SIZE, SIZE), color},
            DebugRenderer::Line {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(-SIZE, SIZE, -SIZE), color},
            DebugRenderer::Line {glm::vec3(-SIZE, -SIZE, -SIZE), glm::vec3(SIZE, -SIZE, -SIZE), color},

            DebugRenderer::Line {glm::vec3(SIZE, -SIZE, -SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            DebugRenderer::Line {glm::vec3(-SIZE, -SIZE, SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},

            DebugRenderer::Line {glm::vec3(SIZE, SIZE, SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            DebugRenderer::Line {glm::vec3(SIZE, SIZE, SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},
            DebugRenderer::Line {glm::vec3(-SIZE, SIZE, -SIZE), glm::vec3(SIZE, SIZE, -SIZE), color},
            DebugRenderer::Line {glm::vec3(-SIZE, SIZE, -SIZE), glm::vec3(-SIZE, SIZE, SIZE), color},

            // Bottom
            DebugRenderer::Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), color},
            DebugRenderer::Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), color},
            DebugRenderer::Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), color},

            DebugRenderer::Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), color},
            DebugRenderer::Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugRenderer::Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), color},

            DebugRenderer::Line {glm::vec3(SIZE2, -SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugRenderer::Line {glm::vec3(-SIZE2, -SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},

            DebugRenderer::Line {glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugRenderer::Line {glm::vec3(SIZE2, SIZE3 + OFFSET, SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},
            DebugRenderer::Line {glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), glm::vec3(SIZE2, SIZE3 + OFFSET, -SIZE2), color},
            DebugRenderer::Line {glm::vec3(-SIZE2, SIZE3 + OFFSET, -SIZE2), glm::vec3(-SIZE2, SIZE3 + OFFSET, SIZE2), color},
        };

        for (const DebugRenderer::Line& line : LINES) {
            debug_add_line(line.position1 + position, line.position2 + position, line.color);
        }
    }

    void Renderer::pre_setup() {
        for (const std::weak_ptr<GlShader>& wshader : storage.scene.shaders) {
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

                const auto uniform_buffer {std::make_shared<GlUniformBuffer>(block)};
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
        storage.scene.shaders.clear();
    }

    void Renderer::render(int width, int height) {
        // TODO see if this can be improved

        {
            const auto uniform_buffer {storage.wprojection_view_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                uniform_buffer->set(&scene.camera.projection_view_matrix, "u_projection_view_matrix"_H);
            }
        }
        {
            const auto uniform_buffer {storage.wdirectional_light_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                uniform_buffer->set(&scene.directional_light.direction, "u_directional_light.direction"_H);
                uniform_buffer->set(&scene.directional_light.ambient_color, "u_directional_light.ambient"_H);
                uniform_buffer->set(&scene.directional_light.diffuse_color, "u_directional_light.diffuse"_H);
                uniform_buffer->set(&scene.directional_light.specular_color, "u_directional_light.specular"_H);
            }
        }
        {
            const auto uniform_buffer {storage.wview_position_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                uniform_buffer->set(&scene.camera.position, "u_view_position"_H);
            }
        }
        {
            const auto uniform_buffer {storage.wpoint_light_uniform_buffer.lock()};

            if (uniform_buffer != nullptr) {
                setup_point_light_uniform_buffer(uniform_buffer);
            }
        }
        {
            const auto uniform_buffer {storage.wlight_space_uniform_buffer.lock()};

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

        opengl::clear(opengl::Buffers::D);
        opengl::viewport(
            storage.shadow_map_framebuffer->get_specification().width,
            storage.shadow_map_framebuffer->get_specification().height
        );

        draw_renderables_to_depth_buffer();

        // Draw normal things
        storage.scene_framebuffer->bind();

        opengl::clear(opengl::Buffers::CDS);
        opengl::viewport(
            storage.scene_framebuffer->get_specification().width,
            storage.scene_framebuffer->get_specification().height
        );

        opengl::bind_texture_2d(storage.shadow_map_framebuffer->get_depth_attachment(), SHADOW_MAP_UNIT);

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

        opengl::viewport(
            storage.intermediate_framebuffer->get_specification().width,
            storage.intermediate_framebuffer->get_specification().height
        );

        end_3d_rendering(width, height);

        // 2D stuff
        draw_texts();
        draw_quads();

#ifndef SM_BUILD_DISTRIBUTION
        debug.render();
#endif
    }

    void Renderer::post_processing() {
        post_processing_context.original_texture = storage.intermediate_framebuffer->get_color_attachment(0);
        post_processing_context.last_texture = post_processing_context.original_texture;
        post_processing_context.textures.clear();

        for (const auto& step : post_processing_context.steps) {
            step->framebuffer->bind();

            opengl::clear(opengl::Buffers::C);
            opengl::viewport(step->framebuffer->get_specification().width, step->framebuffer->get_specification().height);

            step->shader->bind();
            step->setup(post_processing_context);

            sm::opengl::draw_arrays(6);

            post_processing_context.last_texture = step->framebuffer->get_color_attachment(0);
            post_processing_context.textures.push_back(post_processing_context.last_texture);
        }
    }

    void Renderer::end_3d_rendering(int width, int height) {
        opengl::disable_depth_test();

        storage.screen_quad_vertex_array->bind();

        post_processing();

        // Draw the final result to the screen
        GlFramebuffer::bind_default();

        // Clear for debug renderer
        opengl::clear(opengl::Buffers::CD);
        opengl::viewport(width, height);

        screen_quad(post_processing_context.last_texture);

        GlVertexArray::unbind();

        opengl::enable_depth_test();
    }

    void Renderer::screen_quad(unsigned int texture) {
        storage.screen_quad_shader->bind();
        opengl::bind_texture_2d(texture, 0);
        opengl::draw_arrays(6);
    }

    void Renderer::clear() {
        scene.renderables.clear();
        scene.directional_light = {};
        scene.point_lights.clear();
        scene.texts.clear();
        scene.quads.clear();
        storage.text.batches.clear();

#ifndef SM_BUILD_DISTRIBUTION
        debug.clear();
#endif
    }

    void Renderer::resize_framebuffers(int width, int height) {
        if (width == 0 || height == 0) {
            return;
        }

        opengl::viewport(width, height);

        for (std::weak_ptr<GlFramebuffer> wframebuffer : storage.scene.framebuffers) {
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

    void Renderer::draw_renderables() {
        for (const Renderable& renderable : scene.renderables) {
            const auto material {renderable.material.lock()};

            if (material->flags & Material::Outline) {
                continue;  // This one is rendered differently
            }

            if (material->flags & Material::DisableBackFaceCulling) {  // FIXME improve; maybe use scene graph
                opengl::disable_back_face_culling();
                draw_renderable(renderable);
                opengl::enable_back_face_culling();
            } else {
                draw_renderable(renderable);
            }
        }

        GlVertexArray::unbind();
    }

    void Renderer::draw_renderable(const Renderable& renderable) {
        const auto vertex_array {renderable.vertex_array.lock()};
        const auto material {renderable.material.lock()};

        const glm::mat4 matrix {get_renderable_transform(renderable)};

        vertex_array->bind();
        material->bind_and_upload();  // TODO sort and batch models based on material
        material->get_shader()->upload_uniform_mat4("u_model_matrix"_H, matrix);

        opengl::draw_elements(vertex_array->get_index_buffer()->get_index_count());

        // Don't unbind the vertex array
    }

    void Renderer::draw_renderables_outlined() {
        std::vector<Renderable> outline_renderables;

        std::for_each(scene.renderables.cbegin(), scene.renderables.cend(), [&](const Renderable& renderable) {
            const auto material {renderable.material.lock()};

            if (material->flags & Material::Outline) {
                outline_renderables.push_back(renderable);
            }
        });

        std::sort(outline_renderables.begin(), outline_renderables.end(), [this](const Renderable& lhs, const Renderable& rhs) {
            const float distance_left {glm::distance(std::get<0>(lhs.transform).position, scene.camera.position)};
            const float distance_right {glm::distance(std::get<0>(rhs.transform).position, scene.camera.position)};

            return distance_left < distance_right;
        });

        for (const Renderable& renderable : outline_renderables) {
            draw_renderable_outlined(renderable);
        }
    }

    void Renderer::draw_renderable_outlined(const Renderable& renderable) {
        opengl::stencil_mask(0xFF);

        draw_renderable(renderable);

        opengl::stencil_function(opengl::Function::NotEqual, 1, 0xFF);
        opengl::stencil_mask(0x00);

        {
            const auto vertex_array {renderable.vertex_array.lock()};

            glm::mat4 matrix {get_renderable_transform(renderable)};
            matrix = glm::scale(matrix, glm::vec3(renderable.outline.scale));
            matrix = glm::translate(matrix, renderable.outline.offset);

            // Vertex array is already bound

            storage.outline_shader->bind();
            storage.outline_shader->upload_uniform_mat4("u_model_matrix"_H, matrix);
            storage.outline_shader->upload_uniform_vec3("u_color"_H, renderable.outline.color);

            opengl::draw_elements(vertex_array->get_index_buffer()->get_index_count());

            GlVertexArray::unbind();
        }

        opengl::stencil_function(opengl::Function::Always, 1, 0xFF);
        opengl::stencil_mask(0xFF);
    }

    void Renderer::draw_renderables_to_depth_buffer() {
        storage.shadow_shader->bind();

        for (const Renderable& renderable : scene.renderables) {
            const auto vertex_array {renderable.vertex_array.lock()};
            const auto material {renderable.material.lock()};

            if (!(material->flags & Material::CastShadow)) {
                continue;
            }

            const glm::mat4 matrix {get_renderable_transform(renderable)};

            vertex_array->bind();
            storage.shadow_shader->upload_uniform_mat4("u_model_matrix"_H, matrix);

            opengl::draw_elements(vertex_array->get_index_buffer()->get_index_count());
        }

        GlVertexArray::unbind();
    }

    void Renderer::draw_skybox() {
        const glm::mat4& projection {scene.camera.projection_matrix};
        const glm::mat4 view {glm::mat4(glm::mat3(scene.camera.view_matrix))};

        storage.skybox_shader->bind();
        storage.skybox_shader->upload_uniform_mat4("u_projection_view_matrix"_H, projection * view);

        storage.skybox_vertex_array->bind();
        storage.skybox_texture->bind(0);

        opengl::draw_arrays(36);

        GlVertexArray::unbind();
    }

    void Renderer::draw_texts() {
        opengl::disable_depth_test();

        storage.text_shader->bind();

        std::stable_sort(scene.texts.begin(), scene.texts.end(), [](const Text& lhs, const Text& rhs) {
            return lhs.font.lock().get() < rhs.font.lock().get();
        });

        const void* font_ptr {nullptr};  // TODO C++20

        for (const auto& text : scene.texts) {
            const void* this_ptr {text.font.lock().get()};

            assert(this_ptr != nullptr);

            if (this_ptr != font_ptr) {
                font_ptr = this_ptr;

                storage.text.batches.emplace_back().wfont = text.font;
            }

            if (storage.text.batches.back().texts.size() >= SHADER_MAX_BATCH_TEXTS) {
                storage.text.batches.emplace_back().wfont = text.font;
            }

            storage.text.batches.back().texts.push_back(text);
        }

        for (const auto& batch : storage.text.batches) {
            draw_text_batch(batch);
            storage.text.batch_buffer.clear();
            storage.text.batch_matrices.clear();
            storage.text.batch_colors.clear();
        }

        opengl::enable_depth_test();
    }

    void Renderer::draw_text_batch(const TextBatch& batch) {
        const auto font {batch.wfont.lock()};

        std::size_t i {};  // TODO C++20

        for (const Text& text : batch.texts) {
            assert(i < SHADER_MAX_BATCH_TEXTS);

            // Pushes the rendered text onto the buffer
            font->render(text.text, static_cast<int>(i++), storage.text.batch_buffer);

            glm::mat4 matrix {1.0f};  // TODO upload mat3 instead
            matrix = glm::translate(matrix, glm::vec3(text.position, 0.0f));
            matrix = glm::scale(matrix, glm::vec3(std::min(text.scale, 1.0f), std::min(text.scale, 1.0f), 1.0f));

            storage.text.batch_matrices.push_back(matrix);
            storage.text.batch_colors.push_back(text.color);
        }

        // Uniforms must be set as arrays
        storage.text_shader->upload_uniform_mat4_array("u_model_matrix[0]"_H, storage.text.batch_matrices);
        storage.text_shader->upload_uniform_vec3_array("u_color[0]"_H, storage.text.batch_colors);
        storage.text_shader->upload_uniform_mat4("u_projection_matrix"_H, scene.camera_2d.projection_matrix);

        const auto vertex_buffer {storage.wtext_vertex_buffer.lock()};
        vertex_buffer->bind();
        vertex_buffer->upload_data(storage.text.batch_buffer.data(), storage.text.batch_buffer.size());
        GlVertexBuffer::unbind();

        static constexpr std::size_t ITEMS_PER_VERTEX {5};
        static constexpr std::size_t ITEM_SIZE {4};

        assert(storage.text.batch_buffer.size() % (ITEM_SIZE * ITEMS_PER_VERTEX) == 0);

        const int vertex_count {static_cast<int>(storage.text.batch_buffer.size() / (ITEM_SIZE * ITEMS_PER_VERTEX))};

        storage.text_vertex_array->bind();

        opengl::bind_texture_2d(font->get_bitmap()->get_id(), 0);

        opengl::draw_arrays(vertex_count);

        GlVertexArray::unbind();
    }

    void Renderer::draw_quads() {
        opengl::disable_depth_test();

        storage.quad_shader->bind();
        storage.quad_vertex_array->bind();

        storage.quad_shader->upload_uniform_mat4("u_projection_matrix"_H, scene.camera_2d.projection_matrix);

        begin_quads_batch();

        for (const Quad& quad : scene.quads) {
            draw_quad(
                quad.position,
                glm::vec2(static_cast<float>(quad.texture->get_width()), static_cast<float>(quad.texture->get_height())),
                quad.scale,
                quad.texture->get_id()
            );
        }

        end_quads_batch();
        flush_quads_batch();

        GlVertexArray::unbind();

        opengl::enable_depth_test();
    }

    void Renderer::draw_quad(glm::vec2 position, glm::vec2 size, glm::vec2 scale, unsigned int texture) {
        if (storage.quad.quad_count == MAX_QUAD_COUNT || storage.quad.texture_slot_index == MAX_QUADS_TEXTURES) {
            end_quads_batch();
            flush_quads_batch();
            begin_quads_batch();
        }

        int texture_index {-1};

        // Search for this texture in slots array
        for (std::size_t i {0}; i < storage.quad.texture_slot_index; i++) {
            if (storage.quad.texture_slots[i] == texture) {
                texture_index = static_cast<int>(i);
                break;
            }
        }

        if (texture_index < 0) {
            // Not found in slots
            texture_index = static_cast<int>(storage.quad.texture_slot_index);
            storage.quad.texture_slots[storage.quad.texture_slot_index] = texture;
            storage.quad.texture_slot_index++;
        }

        size *= glm::min(scale, glm::vec2(1.0f));

        storage.quad.buffer_pointer->position = glm::vec2(position.x + size.x, position.y + size.y);
        storage.quad.buffer_pointer->texture_coordinate = glm::vec2(1.0f, 1.0f);
        storage.quad.buffer_pointer->texture_index = texture_index;
        storage.quad.buffer_pointer++;

        storage.quad.buffer_pointer->position = glm::vec2(position.x, position.y + size.y);
        storage.quad.buffer_pointer->texture_coordinate = glm::vec2(0.0f, 1.0f);
        storage.quad.buffer_pointer->texture_index = texture_index;
        storage.quad.buffer_pointer++;

        storage.quad.buffer_pointer->position = glm::vec2(position.x, position.y);
        storage.quad.buffer_pointer->texture_coordinate = glm::vec2(0.0f, 0.0f);
        storage.quad.buffer_pointer->texture_index = texture_index;
        storage.quad.buffer_pointer++;

        storage.quad.buffer_pointer->position = glm::vec2(position.x + size.x, position.y);
        storage.quad.buffer_pointer->texture_coordinate = glm::vec2(1.0f, 0.0f);
        storage.quad.buffer_pointer->texture_index = texture_index;
        storage.quad.buffer_pointer++;

        storage.quad.quad_count++;
    }

    void Renderer::begin_quads_batch() {
        storage.quad.quad_count = 0;
        storage.quad.buffer_pointer = storage.quad.buffer.get();
        storage.quad.texture_slot_index = 0;
    }

    void Renderer::end_quads_batch() {
        const std::size_t size {(storage.quad.buffer_pointer - storage.quad.buffer.get()) * sizeof(QuadVertex)};

        const auto vertex_buffer {storage.wquad_vertex_buffer.lock()};

        vertex_buffer->bind();
        vertex_buffer->upload_sub_data(storage.quad.buffer.get(), 0, size);
    }

    void Renderer::flush_quads_batch() {
        for (std::size_t i {0}; i < storage.quad.texture_slot_index; i++) {
            opengl::bind_texture_2d(storage.quad.texture_slots[i], static_cast<int>(i));
        }

        opengl::draw_elements(static_cast<int>(storage.quad.quad_count * 6));
    }

    void Renderer::setup_point_light_uniform_buffer(const std::shared_ptr<GlUniformBuffer> uniform_buffer) {
        // Sort front to back with respect to the camera; lights in the front of the list will be used
        std::sort(
            scene.point_lights.begin(),
            scene.point_lights.end(),
            [this](const PointLight& lhs, const PointLight& rhs) {
                const float distance_left {glm::distance(lhs.position, scene.camera.position)};
                const float distance_right {glm::distance(rhs.position, scene.camera.position)};

                return distance_left < distance_right;
            }
        );

        // Add dummy point lights to make the size 4, which is a requirement from the shader
        if (scene.point_lights.size() < SHADER_MAX_POINT_LIGHTS) {
            scene.point_lights.resize(SHADER_MAX_POINT_LIGHTS);
        }

        for (std::size_t i {0}; i < SHADER_MAX_POINT_LIGHTS; i++) {
            const PointLight& light {scene.point_lights[i]};
            const std::string index {std::to_string(i)};

            // Uniforms must be set individually by index
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
                scene.light_space.left,
                scene.light_space.right,
                scene.light_space.bottom,
                scene.light_space.top,
                scene.light_space.lens_near,
                scene.light_space.lens_far
            )
        };

        const glm::mat4 view {
            glm::lookAt(
                scene.light_space.position,
                scene.directional_light.direction,
                glm::vec3(0.0f, 1.0f, 0.0f)
            )
        };

        const glm::mat4 light_space_matrix {projection * view};

        uniform_buffer->set(&light_space_matrix, "u_light_space_matrix"_H);
    }

    std::shared_ptr<GlIndexBuffer> Renderer::initialize_quads_index_buffer() {
        const auto buffer {std::make_unique<unsigned int[]>(MAX_QUADS_INDICES)};
        unsigned int offset {};

        for (std::size_t i {0}; i < MAX_QUAD_COUNT * 6; i += 6) {
            buffer[i + 0] = 0 + offset;
            buffer[i + 1] = 1 + offset;
            buffer[i + 2] = 2 + offset;
            buffer[i + 3] = 2 + offset;
            buffer[i + 4] = 3 + offset;
            buffer[i + 5] = 0 + offset;

            offset += 4;
        }

        return std::make_shared<GlIndexBuffer>(buffer.get(), MAX_QUADS_INDICES * sizeof(unsigned int));
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
}
