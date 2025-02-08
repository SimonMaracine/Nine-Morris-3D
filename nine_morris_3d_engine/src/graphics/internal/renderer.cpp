#include "nine_morris_3d_engine/graphics/internal/renderer.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <cassert>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/color_space.hpp>
#include <resmanager/resmanager.hpp>

#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/graphics/internal/opengl.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_array.hpp"
#include "nine_morris_3d_engine/graphics/opengl/buffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_buffer_layout.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"
#include "nine_morris_3d_engine/graphics/light.hpp"
#include "nine_morris_3d_engine/graphics/camera.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

// shader uniform limit https://www.khronos.org/opengl/wiki/Uniform_(GLSL)#Implementation_limits
// gamma https://blog.johnnovak.net/2016/09/21/what-every-coder-should-know-about-gamma/
// gamma https://www.cambridgeincolour.com/tutorials/gamma-correction.htm

using namespace resmanager::literals;

namespace sm::internal {
    Renderer::Renderer(int width, int height, const FileSystem& fs, const ShaderLibrary& shd) {
        opengl::initialize_default();
        opengl::initialize_stencil();
        opengl::enable_depth_test();

        {
            FramebufferSpecification specification;
            specification.width = width;
            specification.height = height;
            specification.color_attachments = {
                Attachment(AttachmentFormat::Rgba8Float, AttachmentType::Texture)
            };
            specification.depth_attachment = Attachment(
                AttachmentFormat::Depth32, AttachmentType::Renderbuffer
            );

            m_storage.final_framebuffer = std::make_shared<GlFramebuffer>(specification);

            register_framebuffer(m_storage.final_framebuffer);
        }

        {
            // Doesn't have uniform buffers for sure
            m_storage.screen_quad_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/internal/screen_quad.vert")),
                utils::read_file(fs.path_engine_assets("shaders/internal/screen_quad.frag"))
            );
        }

        {
            m_storage.shadow_shader = std::make_shared<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/internal/shadow.vert")),
                utils::read_file(fs.path_engine_assets("shaders/internal/shadow.frag"))
            );

            register_shader(m_storage.shadow_shader);
        }

        {
            // Doesn't have uniform buffers for sure
            m_storage.text_shader = std::make_unique<GlShader>(
                shd.load_shader(
                    utils::read_file(fs.path_engine_assets("shaders/internal/text.vert")),
                    {{"D_MAX_TEXTS", std::to_string(SHADER_MAX_BATCH_TEXTS)}}
                ),
                shd.load_shader(
                    utils::read_file(fs.path_engine_assets("shaders/internal/text.frag")),
                    {{"D_MAX_TEXTS", std::to_string(SHADER_MAX_BATCH_TEXTS)}}
                )
            );
        }

        {
            // Doesn't have uniform buffers for sure
            m_storage.quad_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/internal/quad.vert")),
                utils::read_file(fs.path_engine_assets("shaders/internal/quad.frag"))
            );

            m_storage.quad_shader->bind();
            m_storage.quad_shader->upload_uniform_int_array("u_texture[0]"_H, {0, 1, 2, 3, 4, 5, 6, 7});  // This is the only way
            GlShader::unbind();
        }

        {
            // Doesn't have uniform buffers for sure
            m_storage.skybox_shader = std::make_unique<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/internal/skybox.vert")),
                utils::read_file(fs.path_engine_assets("shaders/internal/skybox.frag"))
            );
        }

        {
            m_storage.outline_shader = std::make_shared<GlShader>(
                utils::read_file(fs.path_engine_assets("shaders/internal/outline.vert")),
                utils::read_file(fs.path_engine_assets("shaders/internal/outline.frag"))
            );

            register_shader(m_storage.outline_shader);
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

            m_storage.screen_quad_vertex_array = std::make_unique<GlVertexArray>();
            m_storage.screen_quad_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 2);

                va->add_vertex_buffer(vertex_buffer, layout);
            });
        }

        {
            const auto vertex_buffer {std::make_shared<GlVertexBuffer>(CUBEMAP_VERTICES, sizeof(CUBEMAP_VERTICES))};

            m_storage.skybox_vertex_array = std::make_unique<GlVertexArray>();
            m_storage.skybox_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 3);

                va->add_vertex_buffer(vertex_buffer, layout);
            });
        }

        {
            const auto vertex_buffer {std::make_shared<GlVertexBuffer>(DrawHint::Stream)};

            m_storage.text_vertex_array = std::make_unique<GlVertexArray>();
            m_storage.text_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 2);
                layout.add(1, VertexBufferLayout::Float, 2);
                layout.add(2, VertexBufferLayout::Int, 1);

                va->add_vertex_buffer(vertex_buffer, layout);
            });

            m_storage.wtext_vertex_buffer = vertex_buffer;
        }

        {
            const auto vertex_buffer {std::make_shared<GlVertexBuffer>(MAX_QUADS_BUFFER_SIZE, DrawHint::Stream)};
            const auto index_buffer {initialize_quads_index_buffer()};

            m_storage.quad_vertex_array = std::make_unique<GlVertexArray>();
            m_storage.quad_vertex_array->configure([&](GlVertexArray* va) {
                VertexBufferLayout layout;
                layout.add(0, VertexBufferLayout::Float, 2);
                layout.add(1, VertexBufferLayout::Float, 2);
                layout.add(2, VertexBufferLayout::Int, 1);

                va->add_vertex_buffer(vertex_buffer, layout);
                va->add_index_buffer(index_buffer);
            });

            m_storage.wquad_vertex_buffer = vertex_buffer;

            m_storage.quad.buffer = std::make_unique<QuadVertex[]>(MAX_QUAD_COUNT * 4);
        }

#ifndef SM_BUILD_DISTRIBUTION
        debug_initialize(fs);
#endif
    }

    std::shared_ptr<Font> Renderer::get_default_font() const {
        return m_storage.default_font;
    }

    void Renderer::set_color_correction(bool enable) {
        m_color_correction = enable;
    }

    bool Renderer::get_color_correction() const {
        return m_color_correction;
    }

    void Renderer::set_clear_color(glm::vec3 color) {
        if (m_color_correction) {
            m_clear_color = glm::convertSRGBToLinear(color);
        } else {
            m_clear_color = color;
        }

        opengl::clear_color(m_clear_color.r, m_clear_color.g, m_clear_color.b);
    }

    void Renderer::set_samples(int width, int height, int samples) {
        setup_scene_framebuffer(width, height, samples);
    }

    void Renderer::set_scale(const FileSystem& fs, int scale) {
        setup_default_font(fs, scale);
    }

    void Renderer::set_shadow_map_size(int size) {
        setup_shadow_framebuffer(size);
    }

    void Renderer::initialize(int width, int height, const FileSystem& fs, const RendererSpecification& specification) {
        setup_scene_framebuffer(width, height, specification.samples);
        setup_shadow_framebuffer(specification.shadow_map_size);
        setup_default_font(fs, specification.scale);
    }

    void Renderer::register_shader(std::shared_ptr<GlShader> shader) {
        m_storage.shaders.push_back(shader);
        setup_shader_uniform_buffers(shader);
    }

    void Renderer::register_framebuffer(std::shared_ptr<GlFramebuffer> framebuffer) {
        m_storage.framebuffers.push_back(framebuffer);
    }

    void Renderer::render(const Scene& scene, int width, int height) {
        set_and_upload_uniform_buffer_data(scene);

        // Draw to depth buffer for shadows
        m_storage.shadow_map_framebuffer->bind();
        opengl::clear(opengl::Buffers::D);
        opengl::viewport(
            m_storage.shadow_map_framebuffer->get_specification().width,
            m_storage.shadow_map_framebuffer->get_specification().height
        );

        draw_models_to_shadow_map(scene);

        // Draw normal things
        m_storage.scene_framebuffer->bind();
        opengl::clear(opengl::Buffers::CDS);
        opengl::viewport(
            m_storage.scene_framebuffer->get_specification().width,
            m_storage.scene_framebuffer->get_specification().height
        );

        opengl::bind_texture_2d(m_storage.shadow_map_framebuffer->get_depth_attachment(), SHADOW_MAP_UNIT);

        // I sincerely have no idea why outlined objects need to be rendered first; it seems the opposite
        draw_models_outlined(scene);
        draw_models(scene);

        // Skybox is rendered last, but with its depth values modified to keep it in the background
        if (scene.root_node_3d->m_skybox.texture != nullptr) {
            draw_skybox(scene);
        }

        // Blit the resulted scene texture to an intermediate texture, resolving anti-aliasing
        m_storage.scene_framebuffer->blit(
            m_storage.final_framebuffer.get(),
            m_storage.scene_framebuffer->get_specification().width,
            m_storage.scene_framebuffer->get_specification().height
        );

        // Do post processing and render the final 3D image to the screen
        finish_3d(scene, width, height);

        // 2D stuff
        draw_images(scene);
        draw_texts(scene);

#ifndef SM_BUILD_DISTRIBUTION
        debug_render(scene);
#endif
    }

    void Renderer::pre_setup() {
        clear_expired_resources();
    }

    void Renderer::post_setup() {
        clear_expired_resources();
    }

    void Renderer::resize_framebuffers(int width, int height) {
        if (width == 0 || height == 0) {
            return;
        }

        opengl::viewport(width, height);

        for (std::weak_ptr<GlFramebuffer> wframebuffer : m_storage.framebuffers) {
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

    std::size_t Renderer::get_max_point_lights() {
        return SHADER_MAX_POINT_LIGHTS;
    }

    void Renderer::set_and_upload_uniform_buffer_data(const Scene& scene) {
        for (const auto& [binding_index, wuniform_buffer] : m_storage.uniform_buffers) {
            const auto uniform_buffer {wuniform_buffer.lock()};

            if (uniform_buffer == nullptr) {
                continue;
            }

            switch (binding_index) {
                case PROJECTON_VIEW_UNIFORM_BLOCK_BINDING:
                    uniform_buffer->set(&scene.root_node_3d->m_camera.projection_view(), "u_projection_view_matrix"_H);
                    break;
                case DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING:
                    uniform_buffer->set(&scene.root_node_3d->m_directional_light.direction, "u_directional_light.direction"_H);
                    uniform_buffer->set(&scene.root_node_3d->m_directional_light.ambient_color, "u_directional_light.ambient"_H);
                    uniform_buffer->set(&scene.root_node_3d->m_directional_light.diffuse_color, "u_directional_light.diffuse"_H);
                    uniform_buffer->set(&scene.root_node_3d->m_directional_light.specular_color, "u_directional_light.specular"_H);
                    break;
                case VIEW_UNIFORM_BLOCK_BINDING:
                    uniform_buffer->set(&scene.root_node_3d->m_camera_position, "u_view_position"_H);
                    break;
                case POINT_LIGHT_UNIFORM_BLOCK_BINDING:
                    setup_point_light_uniform_buffer(scene, uniform_buffer);
                    break;
                case LIGHT_SPACE_UNIFORM_BLOCK_BINDING:
                    setup_light_space_uniform_buffer(scene, uniform_buffer);
                    break;
                default:
                    assert(false);
                    break;
            }

            uniform_buffer->bind();
            uniform_buffer->upload();
        }

        GlUniformBuffer::unbind();
    }

    void Renderer::post_processing(const Scene& scene) {
        m_post_processing_context.original_texture = m_storage.final_framebuffer->get_color_attachment(0);
        m_post_processing_context.last_texture = m_post_processing_context.original_texture;
        m_post_processing_context.textures.clear();

        for (const auto& step : scene.root_node_3d->m_post_processing_steps) {
            step->m_framebuffer->bind();
            opengl::clear(opengl::Buffers::C);
            opengl::viewport(step->m_framebuffer->get_specification().width, step->m_framebuffer->get_specification().height);

            step->m_shader->bind();
            step->setup(m_post_processing_context);

            opengl::draw_arrays(6);

            m_post_processing_context.last_texture = step->m_framebuffer->get_color_attachment(0);
            m_post_processing_context.textures.push_back(m_post_processing_context.last_texture);
        }
    }

    void Renderer::finish_3d(const Scene& scene, int width, int height) {
        opengl::disable_depth_test();
        opengl::clear_color(0.0f, 0.0f, 0.0f);

        m_storage.screen_quad_vertex_array->bind();

        post_processing(scene);

        GlFramebuffer::bind_default();
        opengl::clear(opengl::Buffers::CD);  // Clear for debug renderer
        opengl::viewport(width, height);

        if (m_color_correction) {
            opengl::enable_framebuffer_srgb();
            screen_quad(m_storage.screen_quad_shader.get(), m_post_processing_context.last_texture);
            opengl::disable_framebuffer_srgb();
        } else {
            screen_quad(m_storage.screen_quad_shader.get(), m_post_processing_context.last_texture);
        }

        GlVertexArray::unbind();

        opengl::clear_color(m_clear_color.r, m_clear_color.g, m_clear_color.b);
        opengl::enable_depth_test();
    }

    void Renderer::screen_quad(const GlShader* shader, unsigned int texture) {
        shader->bind();
        opengl::bind_texture_2d(texture, 0);
        opengl::draw_arrays(6);
    }

    void Renderer::setup_shader_uniform_buffers(std::shared_ptr<GlShader> shader) {
        // Create and store references to particular uniform buffers
        for (const UniformBlockSpecification& block : shader->m_uniform_blocks) {
            // Don't create duplicate buffers
            if (const auto iter {m_storage.uniform_buffers.find(block.binding_index)}; iter != m_storage.uniform_buffers.cend()) {
                if (!iter->second.expired()) {
                    continue;
                }
            }

            const auto uniform_buffer {std::make_shared<GlUniformBuffer>(block)};
            shader->add_uniform_buffer(uniform_buffer);

            m_storage.uniform_buffers[block.binding_index] = uniform_buffer;
        }
    }

    void Renderer::clear_expired_resources() {
        m_storage.shaders.erase(
            std::remove_if(m_storage.shaders.begin(), m_storage.shaders.end(), [](const std::weak_ptr<GlShader>& wshader) {
                return wshader.expired();
            }),
            m_storage.shaders.cend()
        );

        m_storage.framebuffers.erase(
            std::remove_if(m_storage.framebuffers.begin(), m_storage.framebuffers.end(), [](const std::weak_ptr<GlFramebuffer>& wframebuffer) {
                return wframebuffer.expired();
            }),
            m_storage.framebuffers.cend()
        );

        for (auto iter {m_storage.uniform_buffers.begin()}; iter != m_storage.uniform_buffers.end();) {
            if (iter->second.expired()) {
                iter = m_storage.uniform_buffers.erase(iter);
            } else {
                iter++;
            }
        }
    }

    void Renderer::draw_models(const Scene& scene) {
        scene.root_node_3d->traverse([this](const SceneNode3D* node, Context3D& context) {
            auto model_node {dynamic_cast<const ModelNode*>(node)};

            if (model_node == nullptr) {
                return false;
            }

            if (context.outline) {
                return false;  // This one is rendered differently
            }

            if (context.disable_back_face_culling) {
                opengl::disable_back_face_culling();
                draw_model(model_node, context);
                opengl::enable_back_face_culling();
            } else {
                draw_model(model_node, context);
            }

            return false;
        });

        GlVertexArray::unbind();
    }

    void Renderer::draw_model(const ModelNode* model_node, const Context3D& context) {
        model_node->m_vertex_array->bind();
        model_node->m_material->bind_and_upload();
        model_node->m_material->get_shader()->upload_uniform_mat4("u_model_matrix"_H, context.transform_);

        opengl::draw_elements(model_node->m_vertex_array->get_index_buffer()->get_index_count());

        // Don't unbind the vertex array
    }

    void Renderer::draw_models_outlined(const Scene& scene) {
        std::vector<std::pair<const ModelNode*, Context3D>> model_nodes;

        scene.root_node_3d->traverse([this, &model_nodes](const SceneNode3D* node, Context3D& context) {
            auto model_node {dynamic_cast<const ModelNode*>(node)};

            if (model_node == nullptr) {
                return false;
            }

            if (context.outline) {
                model_nodes.emplace_back(model_node, context);
            }

            return false;
        });

        std::sort(model_nodes.begin(), model_nodes.end(), [&](const auto& lhs, const auto& rhs) {
            const float distance_left {glm::distance(lhs.second.transform.position, scene.root_node_3d->m_camera_position)};
            const float distance_right {glm::distance(rhs.second.transform.position, scene.root_node_3d->m_camera_position)};

            return distance_left < distance_right;
        });

        for (const auto& model_node : model_nodes) {
            draw_model_outlined(model_node.first, model_node.second);
        }
    }

    void Renderer::draw_model_outlined(const ModelNode* model_node, const Context3D& context) {
        // Don't disable and enable depth testing

        opengl::stencil_mask(0xFF);

        {
            model_node->m_vertex_array->bind();
            model_node->m_material->bind_and_upload();
            model_node->m_material->get_shader()->upload_uniform_mat4("u_model_matrix"_H, context.transform_);

            opengl::draw_elements(model_node->m_vertex_array->get_index_buffer()->get_index_count());
        }

        opengl::stencil_mask(0x00);
        opengl::stencil_function(opengl::Function::NotEqual, 1, 0xFF);

        {
            // Vertex array is already bound

            const glm::vec3 color {
                m_color_correction ? glm::convertSRGBToLinear(model_node->outline_color) : model_node->outline_color
            };

            m_storage.outline_shader->bind();
            m_storage.outline_shader->upload_uniform_mat4("u_model_matrix"_H, glm::scale(context.transform_, glm::vec3(model_node->outline_thickness)));
            m_storage.outline_shader->upload_uniform_vec3("u_color"_H, color);

            opengl::draw_elements(model_node->m_vertex_array->get_index_buffer()->get_index_count());

            GlVertexArray::unbind();
        }

        opengl::stencil_mask(0xFF);
        opengl::stencil_function(opengl::Function::Always, 1, 0xFF);
    }

    void Renderer::draw_models_to_shadow_map(const Scene& scene) {
        opengl::disable_back_face_culling();

        m_storage.shadow_shader->bind();

        scene.root_node_3d->traverse([this](const SceneNode3D* node, Context3D& context) {
            auto model_node {dynamic_cast<const ModelNode*>(node)};

            if (model_node == nullptr) {
                return false;
            }

            if (!context.cast_shadow) {
                return false;
            }

            model_node->m_vertex_array->bind();

            m_storage.shadow_shader->upload_uniform_mat4("u_model_matrix"_H, context.transform_);

            opengl::draw_elements(model_node->m_vertex_array->get_index_buffer()->get_index_count());

            return false;
        });

        GlVertexArray::unbind();

        opengl::enable_back_face_culling();
    }

    void Renderer::draw_skybox(const Scene& scene) {
        const glm::mat4& projection {scene.root_node_3d->m_camera.projection()};
        const glm::mat4 view {glm::mat4(glm::mat3(scene.root_node_3d->m_camera.view()))};

        m_storage.skybox_shader->bind();
        m_storage.skybox_shader->upload_uniform_mat4("u_projection_view_matrix"_H, projection * view);

        m_storage.skybox_vertex_array->bind();
        scene.root_node_3d->m_skybox.texture->bind(0);

        opengl::draw_arrays(36);

        GlVertexArray::unbind();
    }

    void Renderer::draw_texts(const Scene& scene) {
        opengl::disable_depth_test();

        m_storage.text_shader->bind();

        std::vector<std::pair<const TextNode*, Context2D>> text_nodes;

        scene.root_node_2d->traverse([&text_nodes](const SceneNode2D* node, Context2D& context) {
            auto text_node {dynamic_cast<const TextNode*>(node)};

            if (text_node == nullptr) {
                return false;
            }

            text_nodes.emplace_back(text_node, context);

            return false;
        });

        std::stable_sort(text_nodes.begin(), text_nodes.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.first->m_font.get() < rhs.first->m_font.get();
        });

        for (const void* last {}; const auto& text_node : text_nodes) {
            const void* current {text_node.first->m_font.get()};

            assert(current != nullptr);

            if (current != last) {
                last = current;

                m_storage.text.batches.emplace_back().font = text_node.first->m_font;
            }

            if (m_storage.text.batches.back().texts.size() >= SHADER_MAX_BATCH_TEXTS) {
                m_storage.text.batches.emplace_back().font = text_node.first->m_font;
            }

            m_storage.text.batches.back().texts.push_back(text_node);
        }

        for (const auto& batch : m_storage.text.batches) {
            draw_text_batch(scene, batch);
            m_storage.text.batch_buffer.clear();
            m_storage.text.batch_matrices.clear();
            m_storage.text.batch_colors.clear();
        }

        m_storage.text.batches.clear();

        opengl::enable_depth_test();
    }

    void Renderer::draw_text_batch(const Scene& scene, const TextBatch& batch) {
        for (std::size_t i {0}; const auto& text_node : batch.texts) {
            assert(i < SHADER_MAX_BATCH_TEXTS);

            // Pushes the rendered text onto the buffer
            batch.font->render(text_node.first->text, static_cast<int>(i++), m_storage.text.batch_buffer);

            glm::mat4 matrix {1.0f};  // TODO upload mat3 instead
            matrix = glm::translate(matrix, glm::vec3(text_node.first->transform.position, 0.0f));
            matrix = glm::scale(matrix, glm::vec3(text_node.first->transform.scale, 1.0f));

            m_storage.text.batch_matrices.push_back(matrix);
            m_storage.text.batch_colors.push_back(text_node.first->color);
        }

        // Uniforms must be set as arrays
        m_storage.text_shader->upload_uniform_mat4_array("u_model_matrix[0]"_H, m_storage.text.batch_matrices);
        m_storage.text_shader->upload_uniform_vec3_array("u_color[0]"_H, m_storage.text.batch_colors);
        m_storage.text_shader->upload_uniform_mat4("u_projection_matrix"_H, scene.root_node_2d->m_camera.projection());

        const auto vertex_buffer {m_storage.wtext_vertex_buffer.lock()};
        vertex_buffer->bind();
        vertex_buffer->upload_data(m_storage.text.batch_buffer.data(), m_storage.text.batch_buffer.size() * sizeof(Font::CharacterBuffer));
        GlVertexBuffer::unbind();

        m_storage.text_vertex_array->bind();

        opengl::bind_texture_2d(batch.font->get_bitmap()->get_id(), 0);

        opengl::draw_arrays(static_cast<int>(m_storage.text.batch_buffer.size()) * 6);

        GlVertexArray::unbind();
    }

    void Renderer::draw_images(const Scene& scene) {
        opengl::disable_depth_test();

        m_storage.quad_shader->bind();
        m_storage.quad_vertex_array->bind();

        m_storage.quad_shader->upload_uniform_mat4("u_projection_matrix"_H, scene.root_node_2d->m_camera.projection());

        begin_images_batch();

        scene.root_node_2d->traverse([this](const SceneNode2D* node, Context2D& context) {
            auto image_node {dynamic_cast<const ImageNode*>(node)};

            if (image_node == nullptr) {
                return false;
            }

            draw_image(image_node, context);

            return false;
        });

        end_images_batch();
        flush_images_batch();

        GlVertexArray::unbind();

        opengl::enable_depth_test();
    }

    void Renderer::draw_image(const ImageNode* image_node, const Context2D& context) {
        if (m_storage.quad.quad_count == MAX_QUAD_COUNT || m_storage.quad.texture_index == m_storage.quad.textures.size()) {
            end_images_batch();
            flush_images_batch();
            begin_images_batch();
        }

        int texture_index {-1};
        glm::vec2 size {static_cast<float>(image_node->m_texture->get_width()), static_cast<float>(image_node->m_texture->get_height())};

        // Search for this texture in textures array
        for (std::size_t i {0}; i < m_storage.quad.texture_index; i++) {
            if (m_storage.quad.textures[i] == image_node->m_texture->get_id()) {
                texture_index = static_cast<int>(i);
                break;
            }
        }

        if (texture_index < 0) {
            // Not found in textures
            texture_index = static_cast<int>(m_storage.quad.texture_index);
            m_storage.quad.textures[m_storage.quad.texture_index] = image_node->m_texture->get_id();
            m_storage.quad.texture_index++;
        }

        size *= context.transform.scale;

        m_storage.quad.buffer_pointer->position = glm::vec2(context.transform.position.x + size.x, context.transform.position.y + size.y);
        m_storage.quad.buffer_pointer->texture_coordinate = glm::vec2(1.0f, 1.0f);
        m_storage.quad.buffer_pointer->texture_index = texture_index;
        m_storage.quad.buffer_pointer++;

        m_storage.quad.buffer_pointer->position = glm::vec2(context.transform.position.x, context.transform.position.y + size.y);
        m_storage.quad.buffer_pointer->texture_coordinate = glm::vec2(0.0f, 1.0f);
        m_storage.quad.buffer_pointer->texture_index = texture_index;
        m_storage.quad.buffer_pointer++;

        m_storage.quad.buffer_pointer->position = glm::vec2(context.transform.position.x, context.transform.position.y);
        m_storage.quad.buffer_pointer->texture_coordinate = glm::vec2(0.0f, 0.0f);
        m_storage.quad.buffer_pointer->texture_index = texture_index;
        m_storage.quad.buffer_pointer++;

        m_storage.quad.buffer_pointer->position = glm::vec2(context.transform.position.x + size.x, context.transform.position.y);
        m_storage.quad.buffer_pointer->texture_coordinate = glm::vec2(1.0f, 0.0f);
        m_storage.quad.buffer_pointer->texture_index = texture_index;
        m_storage.quad.buffer_pointer++;

        m_storage.quad.quad_count++;
    }

    void Renderer::begin_images_batch() {
        m_storage.quad.quad_count = 0;
        m_storage.quad.buffer_pointer = m_storage.quad.buffer.get();
        m_storage.quad.texture_index = 0;
    }

    void Renderer::end_images_batch() {
        const std::size_t size {(m_storage.quad.buffer_pointer - m_storage.quad.buffer.get()) * sizeof(QuadVertex)};

        const auto vertex_buffer {m_storage.wquad_vertex_buffer.lock()};

        vertex_buffer->bind();
        vertex_buffer->upload_sub_data(m_storage.quad.buffer.get(), 0, size);
    }

    void Renderer::flush_images_batch() {
        for (std::size_t i {0}; i < m_storage.quad.texture_index; i++) {
            opengl::bind_texture_2d(m_storage.quad.textures[i], static_cast<int>(i));
        }

        opengl::draw_elements(static_cast<int>(m_storage.quad.quad_count * 6));
    }

    void Renderer::setup_point_light_uniform_buffer(const Scene& scene, const std::shared_ptr<GlUniformBuffer> uniform_buffer) {
        // Sort front to back with respect to the camera; lights in the front of the list will be used
        // auto point_lights {scene.m_point_lights};

        std::vector<std::pair<const PointLightNode*, Context3D>> point_light_nodes;

        scene.root_node_3d->traverse([&point_light_nodes](const SceneNode3D* node, Context3D& context) {
            auto point_light_node {dynamic_cast<const PointLightNode*>(node)};

            if (point_light_node == nullptr) {
                return false;
            }

            if (context.outline) {
                point_light_nodes.emplace_back(point_light_node, context);
            }

            return false;
        });

        std::sort(
            point_light_nodes.begin(),
            point_light_nodes.end(),
            [&](const auto& lhs, const auto& rhs) {
                const float distance_left {glm::distance(lhs.second.transform.position, scene.root_node_3d->m_camera_position)};
                const float distance_right {glm::distance(rhs.second.transform.position, scene.root_node_3d->m_camera_position)};

                return distance_left < distance_right;
            }
        );

        // Send dummy point lights to make the size 4, which is a requirement from the shader
        for (std::size_t i {0}; i < SHADER_MAX_POINT_LIGHTS; i++) {
            glm::vec3 position {};
            PointLight point_light;

            if (i + 1 <= point_light_nodes.size()) {
                position = point_light_nodes[i].second.transform.position;
                point_light = *point_light_nodes[i].first;
            }

            const std::string index {std::to_string(i)};

            // Uniforms must be set individually by index
            uniform_buffer->set(&position, Id("u_point_lights[" + index + "].position"));
            uniform_buffer->set(&point_light.ambient_color, Id("u_point_lights[" + index + "].ambient"));
            uniform_buffer->set(&point_light.diffuse_color, Id("u_point_lights[" + index + "].diffuse"));
            uniform_buffer->set(&point_light.specular_color, Id("u_point_lights[" + index + "].specular"));
            uniform_buffer->set(&point_light.falloff_linear, Id("u_point_lights[" + index + "].falloff_linear"));
            uniform_buffer->set(&point_light.falloff_quadratic, Id("u_point_lights[" + index + "].falloff_quadratic"));
        }
    }

    void Renderer::setup_light_space_uniform_buffer(const Scene& scene, std::shared_ptr<GlUniformBuffer> uniform_buffer) {
        const glm::mat4 projection {
            glm::ortho(
                scene.root_node_3d->m_shadow_box.left,
                scene.root_node_3d->m_shadow_box.right,
                scene.root_node_3d->m_shadow_box.bottom,
                scene.root_node_3d->m_shadow_box.top,
                scene.root_node_3d->m_shadow_box.near_,
                scene.root_node_3d->m_shadow_box.far_
            )
        };

        const glm::mat4 view {
            glm::lookAt(
                scene.root_node_3d->m_shadow_box.position,
                scene.root_node_3d->m_directional_light.direction,
                glm::vec3(0.0f, 1.0f, 0.0f)
            )
        };

        const glm::mat4 light_space_matrix {projection * view};

        uniform_buffer->set(&light_space_matrix, "u_light_space_matrix"_H);
    }

    void Renderer::setup_scene_framebuffer(int width, int height, int samples) {
        FramebufferSpecification specification;
        specification.width = width;
        specification.height = height;
        specification.color_attachments = {
            Attachment(AttachmentFormat::Rgba8Float, AttachmentType::Renderbuffer)
        };
        specification.depth_attachment = Attachment(
            AttachmentFormat::Depth24Stencil8, AttachmentType::Renderbuffer
        );
        specification.samples = samples;

        m_storage.scene_framebuffer = std::make_shared<GlFramebuffer>(specification);

        register_framebuffer(m_storage.scene_framebuffer);
    }

    void Renderer::setup_shadow_framebuffer(int size) {
        FramebufferSpecification specification;
        specification.width = size;
        specification.height = size;
        specification.depth_attachment = Attachment(AttachmentFormat::Depth32, AttachmentType::Texture);
        specification.white_border_depth_texture = true;
        specification.resizable = false;

        m_storage.shadow_map_framebuffer = std::make_shared<GlFramebuffer>(specification);

        register_framebuffer(m_storage.shadow_map_framebuffer);
    }

    void Renderer::setup_default_font(const FileSystem& fs, int scale) {
        FontSpecification specification;
        specification.size_height *= static_cast<float>(scale);
        specification.bitmap_size *= scale;

        m_storage.default_font = std::make_unique<Font>(
            utils::read_file(fs.path_engine_assets("fonts/CodeNewRoman/code-new-roman.regular.ttf")),
            specification
        );

        m_storage.default_font->begin_baking();
        m_storage.default_font->bake_ascii();
        m_storage.default_font->end_baking();
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

#ifndef SM_BUILD_DISTRIBUTION
    void Renderer::debug_initialize(const FileSystem& fs) {
        m_debug_storage.shader = std::make_shared<GlShader>(
            utils::read_file(fs.path_engine_assets("shaders/internal/debug.vert")),
            utils::read_file(fs.path_engine_assets("shaders/internal/debug.frag"))
        );

        register_shader(m_debug_storage.shader);

        const auto vertex_buffer {std::make_shared<GlVertexBuffer>(DrawHint::Stream)};
        m_debug_storage.wvertex_buffer = vertex_buffer;

        m_debug_storage.vertex_array = std::make_unique<GlVertexArray>();
        m_debug_storage.vertex_array->configure([&](GlVertexArray* va) {
            VertexBufferLayout layout;
            layout.add(0, VertexBufferLayout::Float, 3);
            layout.add(1, VertexBufferLayout::Float, 3);

            va->add_vertex_buffer(vertex_buffer, layout);
        });
    }

    void Renderer::debug_render(const Scene& scene) {
        for (const DebugLine& line : scene.root_node_3d->m_debug_lines) {
            BufferVertex v1;
            v1.position = line.position1;
            v1.color = line.color;

            m_debug_storage.lines_buffer.push_back(v1);

            BufferVertex v2;
            v2.position = line.position2;
            v2.color = line.color;

            m_debug_storage.lines_buffer.push_back(v2);
        }

        if (m_debug_storage.lines_buffer.empty()) {
            return;
        }

        const auto vertex_buffer {m_debug_storage.wvertex_buffer.lock()};

        vertex_buffer->bind();
        vertex_buffer->upload_data(m_debug_storage.lines_buffer.data(), m_debug_storage.lines_buffer.size() * sizeof(BufferVertex));
        GlVertexBuffer::unbind();

        m_debug_storage.lines_buffer.clear();

        m_debug_storage.shader->bind();
        m_debug_storage.vertex_array->bind();

        opengl::draw_arrays_lines(static_cast<int>(scene.root_node_3d->m_debug_lines.size()) * 2);

        GlVertexArray::unbind();
    }
#endif
}
