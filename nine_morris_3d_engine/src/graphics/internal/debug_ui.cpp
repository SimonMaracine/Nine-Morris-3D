#include "nine_morris_3d_engine/graphics/internal/debug_ui.hpp"

#include <cstring>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "nine_morris_3d_engine/application/context.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"

namespace sm::internal {
#ifndef SM_BUILD_DISTRIBUTION
    void DebugUi::render_dear_imgui(Scene& scene, Ctx& ctx) noexcept {
        if (ImGui::Begin("Debug")) {
            ImGui::Checkbox("Renderables", &renderables);
            ImGui::Checkbox("Lights", &lights);
            ImGui::Checkbox("Shadows", &shadows);
            ImGui::Checkbox("Texts", &texts);
            ImGui::Checkbox("Quads", &quads);
            ImGui::Checkbox("Tasks", &tasks);

            if (ImGui::Checkbox("VSync", &vsync)) {
                ctx.win.set_vsync(static_cast<int>(vsync));
            }
        }

        ImGui::End();

        if (renderables) {
            draw_renderables(scene);
        }

        if (lights) {
            draw_lights(scene);
        }

        if (shadows) {
            draw_shadows(scene);
        }

        if (texts) {
            draw_texts(scene);
        }

        if (quads) {
            draw_quads(scene);
        }

        if (tasks) {
            draw_tasks(ctx);
        }
    }

    void DebugUi::add_lines(Scene& scene) {
        if (shadows) {
            add_shadows_lines(
                scene,
                scene.m_debug.shadow_box->left,
                scene.m_debug.shadow_box->right,
                scene.m_debug.shadow_box->bottom,
                scene.m_debug.shadow_box->top,
                scene.m_debug.shadow_box->near_,
                scene.m_debug.shadow_box->far_,
                scene.m_debug.shadow_box->position,
                scene.m_debug.directional_light->direction
            );
        }
    }

    void DebugUi::draw_renderables(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Renderables")) {
            int index {};  // TODO C++20

            for (Renderable* renderable : scene.m_debug.renderables) {
                ImGui::PushID(index);
                ImGui::Text("Renderable %d", index);
                ImGui::DragFloat3("Position", glm::value_ptr(renderable->transform.position), 1.0f, -200.0f, 200.0f);
                ImGui::DragFloat3("Rotation", glm::value_ptr(renderable->transform.rotation), 1.0f, 0.0f, 360.0f);
                ImGui::DragFloat("Scale", &renderable->transform.scale, 0.1f, 0.0f, 100.0f);
                ImGui::PopID();
                ImGui::Spacing();

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::draw_lights(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Directional Light")) {
            ImGui::DragFloat3("Direction", glm::value_ptr(scene.m_debug.directional_light->direction), 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat3("Ambient", glm::value_ptr(scene.m_debug.directional_light->ambient_color), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Diffuse", glm::value_ptr(scene.m_debug.directional_light->diffuse_color), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Specular", glm::value_ptr(scene.m_debug.directional_light->specular_color), 0.01f, 0.0f, 1.0f);
        }

        ImGui::End();

        if (ImGui::Begin("Debug Point Lights")) {
            int index {};  // TODO C++20

            for (PointLight* point_light : scene.m_debug.point_lights) {
                ImGui::PushID(index);
                ImGui::Text("Light %d", index);
                ImGui::DragFloat3("Position", glm::value_ptr(point_light->position), 1.0f, -30.0f, 30.0f);
                ImGui::DragFloat3("Ambient", glm::value_ptr(point_light->ambient_color), 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat3("Diffuse", glm::value_ptr(point_light->diffuse_color), 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat3("Specular", glm::value_ptr(point_light->specular_color), 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Falloff L", &point_light->falloff_linear, 0.0001f, 0.0001f, 1.0f);
                ImGui::DragFloat("Falloff Q", &point_light->falloff_quadratic, 0.00001f, 0.00001f, 1.0f);
                ImGui::PopID();
                ImGui::Spacing();

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::draw_shadows(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Shadows")) {
            ImGui::DragFloat("Left", &scene.m_debug.shadow_box->left, 1.0f, -500.0f, 0.0f);
            ImGui::DragFloat("Right", &scene.m_debug.shadow_box->right, 1.0f, 0.0f, 500.0f);
            ImGui::DragFloat("Bottom", &scene.m_debug.shadow_box->bottom, 1.0f, -500.0f, 0.0f);
            ImGui::DragFloat("Top", &scene.m_debug.shadow_box->top, 1.0f, 0.0f, 500.0f);
            ImGui::DragFloat("Near", &scene.m_debug.shadow_box->near_, 1.0f, 0.1f, 2.0f);
            ImGui::DragFloat("Far", &scene.m_debug.shadow_box->far_, 1.0f, 2.0f, 500.0f);
            ImGui::Text(
                "Position %f, %f, %f",
                scene.m_debug.shadow_box->position.x,
                scene.m_debug.shadow_box->position.y,
                scene.m_debug.shadow_box->position.z
            );
        }

        ImGui::End();
    }

    void DebugUi::draw_texts(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Texts")) {
            int index {};  // TODO C++20

            for (Text* text : scene.m_debug.texts) {
                char buffer[512] {};
                std::strncpy(buffer, text->text.c_str(), sizeof(buffer) - 1);

                ImGui::PushID(index);
                ImGui::Text("Text %d", index);
                ImGui::DragFloat2("Position", glm::value_ptr(text->position), 1.0f, -2000.0f, 2000.0f);
                ImGui::DragFloat("Scale", &text->scale, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat3("Color", glm::value_ptr(text->color), 1.0f, 0.0f, 1.0f);
                ImGui::InputTextMultiline("Text", buffer, sizeof(buffer));
                ImGui::PopID();
                ImGui::Spacing();

                text->text = buffer;

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::draw_quads(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Quads")) {
            int index {};  // TODO C++20

            for (Quad* quad : scene.m_debug.quads) {
                ImGui::PushID(index);
                ImGui::Text("Quad %d", index);
                ImGui::DragFloat2("Position", glm::value_ptr(quad->position), 1.0f, -2000.0f, 2000.0f);
                ImGui::DragFloat2("Scale", glm::value_ptr(quad->scale), 0.01f, 0.0f, 1.0f);
                ImGui::PopID();
                ImGui::Spacing();

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::draw_tasks(Ctx& ctx) noexcept {
        if (ImGui::Begin("Debug Tasks")) {
            ImGui::Text("Tasks count %lu", ctx.tsk.tasks_active.size());
            ImGui::Text("Async tasks count %lu", ctx.tsk.async_tasks.size());
        }

        ImGui::End();
    }

    void DebugUi::add_shadows_lines(
        Scene& scene,
        float left,
        float right,
        float bottom,
        float top,
        float near,
        float far,
        glm::vec3 position,
        glm::vec3 orientation
    ) {
        const glm::vec3 color {1.0f};
        const glm::vec3 third {glm::cross(orientation, glm::vec3(0.0f, 1.0f, 0.0f))};
        const glm::vec3 third2 {-glm::cross(orientation, glm::cross(orientation, glm::vec3(0.0f, 1.0f, 0.0f)))};

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * top,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * top,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third2) * bottom + glm::normalize(third) * left,
            position + glm::normalize(orientation) * near + glm::normalize(third2) * bottom + glm::normalize(third) * right,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third2) * top + glm::normalize(third) * left,
            position + glm::normalize(orientation) * near + glm::normalize(third2) * top + glm::normalize(third) * right,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * top,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * top,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third2) * bottom + glm::normalize(third) * left,
            position + glm::normalize(orientation) * far + glm::normalize(third2) * bottom + glm::normalize(third) * right,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third2) * top + glm::normalize(third) * left,
            position + glm::normalize(orientation) * far + glm::normalize(third2) * top + glm::normalize(third) * right,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * top,
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * top,
            color
        );

        scene.debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * top,
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * top,
            color
        );
    }
#endif
}
