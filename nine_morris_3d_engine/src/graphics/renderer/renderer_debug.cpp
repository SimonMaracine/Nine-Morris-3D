#include <vector>
#include <memory>
#include <cstddef>
#include <array>

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/vertex_buffer_layout.hpp"
#include "engine/graphics/renderer/renderer_debug.hpp"
#include "engine/graphics/renderer/render_gl.hpp"
#include "engine/other/file_system.hpp"
#include "engine/other/encrypt.hpp"
#include "engine/other/assert.hpp"

using namespace resmanager::literals;

namespace sm {
    RendererDebug::RendererDebug() {
        storage.shader = std::make_unique<GlShader>(
            Encrypt::encr(FileSystem::path_engine_data("shaders/debug.vert")),
            Encrypt::encr(FileSystem::path_engine_data("shaders/debug.frag"))
        );

        auto vertex_buffer = std::make_shared<GlVertexBuffer>(DrawHint::Stream);
        storage.vertex_buffer = vertex_buffer;

        VertexBufferLayout layout;
        layout.add(0, VertexBufferLayout::Float, 3);
        layout.add(1, VertexBufferLayout::Float, 3);

        storage.vertex_array = std::make_unique<GlVertexArray>();
        storage.vertex_array->bind();
        storage.vertex_array->add_vertex_buffer(vertex_buffer, layout);
        GlVertexArray::unbind();
    }

    RendererDebug::~RendererDebug() {

    }

    void RendererDebug::add_line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color) {
        Line line;
        line.p1 = p1;
        line.p2 = p2;
        line.color = color;

        scene_list.lines.push_back(line);
    }

    void RendererDebug::add_lines(const std::vector<glm::vec3>& points, const glm::vec3& color) {
        SM_ASSERT(points.size() >= 2, "There must be at least one line");

        Line line;
        line.color = color;

        for (std::size_t i = 1; i < points.size(); i++) {
            line.p1 = points[i - 1];
            line.p2 = points[i];

            scene_list.lines.push_back(line);
        }
    }

    void RendererDebug::add_point(const glm::vec3& p, const glm::vec3& color) {
        static constexpr float SIZE = 0.5f;

        add_line(glm::vec3(-SIZE, 0.0f, 0.0f) + p, glm::vec3(SIZE, 0.0f, 0.0f) + p, color);
        add_line(glm::vec3(0.0f, -SIZE, 0.0f) + p, glm::vec3(0.0f, SIZE, 0.0f) + p, color);
        add_line(glm::vec3(0.0f, 0.0f, -SIZE) + p, glm::vec3(0.0f, 0.0f, SIZE) + p, color);
    }

    void RendererDebug::add_lamp(const glm::vec3& position, const glm::vec3& color) {
        static constexpr float SIZE = 0.4f;
        static constexpr float SIZE2 = 0.2f;
        static constexpr float SIZE3 = 0.6f;
        static constexpr float OFFSET = -(SIZE + SIZE3);
        const std::array<Line, 24> LINES = {
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
            add_line(line.p1 + position, line.p2 + position, line.color);
        }
    }

    void RendererDebug::render_3d(const glm::mat4& projection_view_matrix) {
        static std::vector<BufferVertexStruct> buffer;
        buffer.clear();

        for (const Line& line : scene_list.lines) {
            BufferVertexStruct v1;
            v1.position = line.p1;
            v1.color = line.color;

            buffer.push_back(v1);

            BufferVertexStruct v2;
            v2.position = line.p2;
            v2.color = line.color;

            buffer.push_back(v2);
        }

        if (buffer.empty()) {
            return;
        }

        auto vertex_buffer = storage.vertex_buffer.lock();

        vertex_buffer->bind();
        vertex_buffer->upload_data(buffer.data(), buffer.size() * sizeof(BufferVertexStruct));
        GlVertexBuffer::unbind();

        storage.shader->bind();
        storage.shader->upload_uniform_mat4("u_projection_view_matrix"_H, projection_view_matrix);

        storage.vertex_array->bind();

        RenderGl::draw_arrays_lines(scene_list.lines.size() * 2);

        GlVertexArray::unbind();

        scene_list.clear();
    }

    void RendererDebug::SceneList::clear() {
        lines.clear();
    }
}
