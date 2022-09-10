#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/events.h"
#include "nine_morris_3d_engine/graphics/renderer/renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/gui_renderer.h"
#include "nine_morris_3d_engine/graphics/renderer/camera.h"
#include "nine_morris_3d_engine/graphics/renderer/hover.h"
#include "nine_morris_3d_engine/graphics/renderer/material.h"

struct TransformComponent {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(0.0f);
};

struct TagComponent {  // TODO maybe not needed
    std::string tag;
};

struct RenderComponent {};

struct ModelComponent {
    entt::resource_handle<VertexArray> vertex_array;
    int index_count = 0;
    entt::resource_handle<MaterialInstance> material;
};

struct OutlineComponent {
    bool outline_enabled = false;
    glm::vec3 outline_color = glm::vec3(0.0f);
};

struct HoverComponent {
    hover::Id id = hover::null;
};

struct CastShadowComponent {};

struct QuadComponent {
    std::shared_ptr<Texture> texture;
};

struct GuiImageComponent {
    std::shared_ptr<gui::Image> image;
};

struct GuiTextComponent {
    std::shared_ptr<gui::Text> text;
};

struct GuiRenderComponent {
    GuiRenderer* gui_renderer = nullptr;
};

struct GuiOffsetComponent {
    std::vector<std::tuple<unsigned int, gui::Relative>> offsets;
};

struct GuiStickComponent {
    gui::Sticky sticky = gui::Sticky::Center;
};

struct GuiScaleComponent {
    float min_scale = 0.0f;
    float max_scale = 0.0f;
    int min_bound = 0.0f;
    int max_bound = 0.0f;
};

struct CameraComponent {
    Camera camera;
};
