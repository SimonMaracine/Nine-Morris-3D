#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "engine/graphics/opengl/vertex_array.hpp"
#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/shader.hpp"

namespace sm {
    class Application;

    class RendererDebug {
    public:
        RendererDebug();
        ~RendererDebug();

        RendererDebug(const RendererDebug&) = delete;
        RendererDebug& operator=(const RendererDebug&) = delete;
        RendererDebug(RendererDebug&&) = delete;
        RendererDebug& operator=(RendererDebug&&) = delete;

        void add_line(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color);
        void add_lines(const std::vector<glm::vec3>& points, const glm::vec3& color);
        void add_point(const glm::vec3& p, const glm::vec3& color);

        void add_lamp(const glm::vec3& position, const glm::vec3& color);
    private:
        void render_3d(const glm::mat4& projectio_view_matrix);

        struct {
            std::unique_ptr<GlShader> shader;

            std::weak_ptr<GlVertexBuffer> vertex_buffer;
            std::unique_ptr<GlVertexArray> vertex_array;
        } storage;

        struct BufferVertexStruct {
            glm::vec3 position;
            glm::vec3 color;
        };

        struct Line {
            glm::vec3 p1;
            glm::vec3 p2;
            glm::vec3 color;
        };

        struct SceneList {
            std::vector<Line> lines;

            void clear();
        } scene_list;

        friend class Application;
    };
}
