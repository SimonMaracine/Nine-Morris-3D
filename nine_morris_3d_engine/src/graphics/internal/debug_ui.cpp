#include "nine_morris_3d_engine/graphics/internal/debug_ui.hpp"

#include <cstring>
#include <cstdio>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "nine_morris_3d_engine/application/context.hpp"

namespace sm::internal {
#ifndef SM_BUILD_DISTRIBUTION
    void DebugUi::render(Scene& scene, Ctx& ctx) noexcept {
        if (ImGui::Begin("Debug")) {
            ImGui::Checkbox("Renderables", &m_renderables);
            ImGui::Checkbox("Lights", &m_lights);
            ImGui::Checkbox("Shadows", &m_shadows);
            ImGui::Checkbox("Texts", &m_texts);
            ImGui::Checkbox("Quads", &m_quads);
            ImGui::Checkbox("Tasks", &m_tasks);
            ImGui::Checkbox("Frame Time", &m_frame_time);

            if (ImGui::Checkbox("VSync", &m_vsync)) {
                ctx.m_win.set_vsync(m_vsync);
            }
        }

        ImGui::End();

        if (m_renderables) {
            renderables(scene);
        }

        if (m_lights) {
            lights(scene);
        }

        if (m_shadows) {
            shadows(scene);
        }

        if (m_texts) {
            texts(scene);
        }

        if (m_quads) {
            quads(scene);
        }

        if (m_tasks) {
            tasks(ctx);
        }

        if (m_frame_time) {
            frame_time(ctx);
        }
    }

    void DebugUi::render_lines(Scene& scene) {
        if (m_shadows) {
            shadows_lines(
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

    void DebugUi::renderables(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Renderables")) {
            for (int index {0}; Renderable* renderable : scene.m_debug.renderables) {
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

    void DebugUi::lights(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Directional Light")) {
            ImGui::DragFloat3("Direction", glm::value_ptr(scene.m_debug.directional_light->direction), 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat3("Ambient", glm::value_ptr(scene.m_debug.directional_light->ambient_color), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Diffuse", glm::value_ptr(scene.m_debug.directional_light->diffuse_color), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Specular", glm::value_ptr(scene.m_debug.directional_light->specular_color), 0.01f, 0.0f, 1.0f);
        }

        ImGui::End();

        if (ImGui::Begin("Debug Point Lights")) {
            for (int index {0}; PointLight* point_light : scene.m_debug.point_lights) {
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

    void DebugUi::shadows(Scene& scene) noexcept {
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

    void DebugUi::texts(Scene& scene) {
        if (ImGui::Begin("Debug Texts")) {
            for (int index {0}; Text* text : scene.m_debug.texts) {
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

    void DebugUi::quads(Scene& scene) noexcept {
        if (ImGui::Begin("Debug Quads")) {
            for (int index {0}; Quad* quad : scene.m_debug.quads) {
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

    void DebugUi::tasks(Ctx& ctx) noexcept {
        if (ImGui::Begin("Debug Tasks")) {
            ImGui::Text("Tasks count: %lu", ctx.m_tsk.m_tasks_active.size());
            ImGui::Text("Async tasks count: %lu", ctx.m_tsk.m_async_tasks.size());
        }

        ImGui::End();
    }

    void DebugUi::frame_time(Ctx& ctx) {
        const float time {ctx.get_delta() * 1000.0f};
        frames[index] = time;

        if (index < FRAMES_SIZE) {
            index++;
            frames.push_back(time);
        } else {
            frames.push_back(time);
            frames.erase(frames.cbegin());
        }

        char text[32] {};
        std::snprintf(text, sizeof(text), "%.3f", time);

        if (ImGui::Begin("Frame Time")) {
            ImGui::PlotLines("time (ms)", frames.data(), FRAMES_SIZE, 0, text, 0.0f, 50.0f, ImVec2(200, 60));
        }

        ImGui::End();
    }

    void DebugUi::shadows_lines(
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
