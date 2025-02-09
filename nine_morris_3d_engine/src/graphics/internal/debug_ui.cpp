#include "nine_morris_3d_engine/graphics/internal/debug_ui.hpp"

#include <cstring>
#include <cstdio>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "nine_morris_3d_engine/application/context.hpp"

namespace sm::internal {
#ifndef SM_BUILD_DISTRIBUTION
    void DebugUi::render(const Scene& scene, Ctx& ctx) {
        if (ImGui::Begin("Debug")) {
            ImGui::Checkbox("Models", &m_models);
            ImGui::Checkbox("Lights", &m_lights);
            ImGui::Checkbox("Shadows", &m_shadows);
            ImGui::Checkbox("Images", &m_images);
            ImGui::Checkbox("Texts", &m_texts);
            ImGui::Checkbox("Tasks", &m_tasks);
            ImGui::Checkbox("Frame Time", &m_frame_time);

            if (ImGui::Checkbox("VSync", &m_vsync)) {
                ctx.m_win.set_vsync(m_vsync);
            }
        }

        ImGui::End();

        m_model_nodes.clear();
        m_point_light_nodes.clear();
        m_image_nodes.clear();
        m_text_nodes.clear();

        scene.root_node_3d->traverse([this](SceneNode3D* node) {
            switch (node->type()) {
                case SceneNode3DType::Root3D:
                    break;
                case SceneNode3DType::Model:
                    m_model_nodes.push_back(static_cast<ModelNode*>(node));
                    break;
                case SceneNode3DType::PointLight:
                    m_point_light_nodes.push_back(static_cast<PointLightNode*>(node));
                    break;
            }

            return false;
        });

        scene.root_node_2d->traverse([this](SceneNode2D* node) {
            switch (node->type()) {
                case SceneNode2DType::Root2D:
                    break;
                case SceneNode2DType::Image:
                    m_image_nodes.push_back(static_cast<ImageNode*>(node));
                    break;
                case SceneNode2DType::Text:
                    m_text_nodes.push_back(static_cast<TextNode*>(node));
                    break;
            }

            return false;
        });

        if (m_models) {
            models(scene);
        }

        if (m_lights) {
            lights(scene);
        }

        if (m_shadows) {
            shadows(scene);
        }

        if (m_images) {
            images(scene);
        }

        if (m_texts) {
            texts(scene);
        }

        if (m_tasks) {
            tasks(ctx);
        }

        if (m_frame_time) {
            frame_time(ctx);
        }
    }

    void DebugUi::render_lines(const Scene& scene) {
        if (m_shadows) {
            shadows_lines(
                scene,
                scene.root_node_3d->shadow_box.left,
                scene.root_node_3d->shadow_box.right,
                scene.root_node_3d->shadow_box.bottom,
                scene.root_node_3d->shadow_box.top,
                scene.root_node_3d->shadow_box.near_,
                scene.root_node_3d->shadow_box.far_,
                scene.root_node_3d->shadow_box.position,
                scene.root_node_3d->directional_light.direction
            );
        }
    }

    void DebugUi::models(const Scene&) {
        if (ImGui::Begin("Debug Models")) {
            for (int index {0}; const auto& model_node : m_model_nodes) {
                ImGui::PushID(index);
                ImGui::Text("Model %d", index);
                ImGui::DragFloat3("Position", glm::value_ptr(model_node->position), 0.01f, -200.0f, 200.0f);
                ImGui::DragFloat3("Rotation", glm::value_ptr(model_node->rotation), 1.0f, -360.0f, 360.0f);
                ImGui::DragFloat("Scale", &model_node->scale, 0.01f, 0.0f, 100.0f);
                ImGui::PopID();
                ImGui::Spacing();

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::lights(const Scene& scene) {
        if (ImGui::Begin("Debug Directional Light")) {
            ImGui::DragFloat3("Direction", glm::value_ptr(scene.root_node_3d->directional_light.direction), 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat3("Ambient", glm::value_ptr(scene.root_node_3d->directional_light.ambient_color), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Diffuse", glm::value_ptr(scene.root_node_3d->directional_light.diffuse_color), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Specular", glm::value_ptr(scene.root_node_3d->directional_light.specular_color), 0.01f, 0.0f, 1.0f);

            // Direction should stay normalized no matter what
            scene.root_node_3d->directional_light.direction = glm::normalize(scene.root_node_3d->directional_light.direction);
        }

        ImGui::End();

        if (ImGui::Begin("Debug Point Lights")) {
            for (int index {0}; const auto& point_light_node : m_point_light_nodes) {
                ImGui::PushID(index);
                ImGui::Text("Light %d", index);
                ImGui::DragFloat3("Position", glm::value_ptr(point_light_node->position), 0.01f, -30.0f, 30.0f);
                ImGui::DragFloat3("Ambient", glm::value_ptr(point_light_node->ambient_color), 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat3("Diffuse", glm::value_ptr(point_light_node->diffuse_color), 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat3("Specular", glm::value_ptr(point_light_node->specular_color), 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Falloff L", &point_light_node->falloff_linear, 0.0001f, 0.0001f, 1.0f);
                ImGui::DragFloat("Falloff Q", &point_light_node->falloff_quadratic, 0.00001f, 0.00001f, 1.0f);
                ImGui::PopID();
                ImGui::Spacing();

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::shadows(const Scene& scene) {
        if (ImGui::Begin("Debug Shadows")) {
            ImGui::DragFloat("Left", &scene.root_node_3d->shadow_box.left, 0.01f, -500.0f, 0.0f);
            ImGui::DragFloat("Right", &scene.root_node_3d->shadow_box.right, 0.01f, 0.0f, 500.0f);
            ImGui::DragFloat("Bottom", &scene.root_node_3d->shadow_box.bottom, 0.01f, -500.0f, 0.0f);
            ImGui::DragFloat("Top", &scene.root_node_3d->shadow_box.top, 0.01f, 0.0f, 500.0f);
            ImGui::DragFloat("Near", &scene.root_node_3d->shadow_box.near_, 0.01f, 0.1f, 2.0f);
            ImGui::DragFloat("Far", &scene.root_node_3d->shadow_box.far_, 0.01f, 2.0f, 500.0f);
            ImGui::Text(
                "Position %f, %f, %f",
                scene.root_node_3d->shadow_box.position.x,
                scene.root_node_3d->shadow_box.position.y,
                scene.root_node_3d->shadow_box.position.z
            );
        }

        ImGui::End();
    }

    void DebugUi::images(const Scene&) {
        if (ImGui::Begin("Debug Images")) {
            for (int index {0}; const auto& image_node : m_image_nodes) {
                ImGui::PushID(index);
                ImGui::Text("Image %d", index);
                ImGui::DragFloat2("Position", glm::value_ptr(image_node->position), 0.01f, -2000.0f, 2000.0f);
                ImGui::DragFloat2("Scale", glm::value_ptr(image_node->scale), 0.01f, 0.0f, 1.0f);
                ImGui::PopID();
                ImGui::Spacing();

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::texts(const Scene&) {
        if (ImGui::Begin("Debug Texts")) {
            for (int index {0}; const auto& text_node : m_text_nodes) {
                char buffer[512] {};
                std::strncpy(buffer, text_node->text.c_str(), sizeof(buffer) - 1);

                ImGui::PushID(index);
                ImGui::Text("Text %d", index);
                ImGui::DragFloat2("Position", glm::value_ptr(text_node->position), 0.01f, -2000.0f, 2000.0f);
                ImGui::DragFloat2("Scale", &text_node->scale, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat3("Color", glm::value_ptr(text_node->color), 0.001f, 0.0f, 1.0f);
                ImGui::InputTextMultiline("Text", buffer, sizeof(buffer));
                ImGui::PopID();
                ImGui::Spacing();

                text_node->text = buffer;

                index++;
            }
        }

        ImGui::End();
    }

    void DebugUi::tasks(Ctx& ctx) {
        if (ImGui::Begin("Debug Tasks")) {
            ImGui::Text("Tasks count: %lu", ctx.m_tsk.m_tasks_active.size());
            ImGui::Text("Async tasks count: %lu", ctx.m_tsk.m_async_tasks.size());
        }

        ImGui::End();
    }

    void DebugUi::frame_time(Ctx& ctx) {
        const float time {ctx.get_delta() * 1000.0f};
        m_frames[m_index] = time;

        if (m_index < FRAMES_SIZE) {
            m_index++;
            m_frames.push_back(time);
        } else {
            m_frames.push_back(time);
            m_frames.erase(m_frames.cbegin());
        }

        char text[32] {};
        std::snprintf(text, sizeof(text), "%.3f", time);

        if (ImGui::Begin("Frame Time")) {
            ImGui::PlotLines("time (ms)", m_frames.data(), FRAMES_SIZE, 0, text, 0.0f, 50.0f, ImVec2(200, 60));
        }

        ImGui::End();
    }

    void DebugUi::shadows_lines(
        const Scene& scene,
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

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * top,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * top,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third2) * bottom + glm::normalize(third) * left,
            position + glm::normalize(orientation) * near + glm::normalize(third2) * bottom + glm::normalize(third) * right,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third2) * top + glm::normalize(third) * left,
            position + glm::normalize(orientation) * near + glm::normalize(third2) * top + glm::normalize(third) * right,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * top,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * top,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third2) * bottom + glm::normalize(third) * left,
            position + glm::normalize(orientation) * far + glm::normalize(third2) * bottom + glm::normalize(third) * right,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * far + glm::normalize(third2) * top + glm::normalize(third) * left,
            position + glm::normalize(orientation) * far + glm::normalize(third2) * top + glm::normalize(third) * right,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * bottom,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * bottom,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * left + glm::normalize(third2) * top,
            position + glm::normalize(orientation) * far + glm::normalize(third) * left + glm::normalize(third2) * top,
            color
        );

        scene.root_node_3d->debug_add_line(
            position + glm::normalize(orientation) * near + glm::normalize(third) * right + glm::normalize(third2) * top,
            position + glm::normalize(orientation) * far + glm::normalize(third) * right + glm::normalize(third2) * top,
            color
        );
    }
#endif
}
