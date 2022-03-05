#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "graphics/renderer/shader.h"
#include "graphics/renderer/vertex_array.h"

class Application;

class GuiRenderer {
public:
    GuiRenderer(Application* app);
    ~GuiRenderer();

    void render();
private:
    struct Storage {
        std::shared_ptr<Shader> quad2d_shader;
        std::shared_ptr<Shader> text_shader;

        std::shared_ptr<VertexArray> quad2d_vertex_array;

        glm::mat4 orthographic_projection_matrix = glm::mat4(1.0f);
    } storage;

    const char* QUAD2D_VERTEX_SHADER = "data/shaders/internal/quad2d.vert";
    const char* QUAD2D_FRAGMENT_SHADER = "data/shaders/internal/quad2d.frag";
    const char* TEXT_VERTEX_SHADER = "data/shaders/internal/text.vert";
    const char* TEXT_FRAGMENT_SHADER = "data/shaders/internal/text.frag";

    // Reference to application
    Application* app = nullptr;

    friend class Application;
};
