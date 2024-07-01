#include "nine_morris_3d_engine/graphics/debug_ui.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "nine_morris_3d_engine/graphics/scene.hpp"

namespace sm {
    void DebugUi::render_dear_imgui(Scene& scene) {
        if (ImGui::Begin("Debug")) {
            ImGui::Checkbox("Shadows", &shadows);
            ImGui::Checkbox("Lights", &lights);
        }

        ImGui::End();

        if (shadows) {
            draw_shadows(scene);
        }

        if (lights) {
            draw_lights(scene);
        }
    }

    void DebugUi::add_lines(Scene& scene) {
        if (shadows) {
            draw_shadows_lines(
                scene,
                scene.light_space.left,
                scene.light_space.right,
                scene.light_space.bottom,
                scene.light_space.top,
                scene.light_space.near,
                scene.light_space.far,
                scene.light_space.position,
                scene.directional_light.direction
            );
        }
    }

    void DebugUi::draw_shadows(Scene& scene) {
        if (ImGui::Begin("Debug Shadows")) {
            ImGui::SliderFloat("Left", &scene.light_space.left, -500.0f, 0.0f);
            ImGui::SliderFloat("Right", &scene.light_space.right, 0.0f, 500.0f);
            ImGui::SliderFloat("Bottom", &scene.light_space.bottom, -500.0f, 0.0f);
            ImGui::SliderFloat("Top", &scene.light_space.top, 0.0f, 500.0f);
            ImGui::SliderFloat("Near", &scene.light_space.near, 0.1f, 2.0f);
            ImGui::SliderFloat("Far", &scene.light_space.far, 2.0f, 500.0f);
            ImGui::Text("Position %f, %f, %f", scene.light_space.position.x, scene.light_space.position.y, scene.light_space.position.z);
        }

        ImGui::End();
    }

    void DebugUi::draw_lights(Scene& scene) {
        if (ImGui::Begin("Debug Directional Light")) {
            ImGui::SliderFloat3("Direction", glm::value_ptr(scene.directional_light.direction), -1.0f, 1.0f);
            ImGui::SliderFloat3("Ambient", glm::value_ptr(scene.directional_light.ambient_color), 0.0f, 1.0f);
            ImGui::SliderFloat3("Diffuse", glm::value_ptr(scene.directional_light.diffuse_color), 0.0f, 1.0f);
            ImGui::SliderFloat3("Specular", glm::value_ptr(scene.directional_light.specular_color), 0.0f, 1.0f);
        }

        ImGui::End();

        if (ImGui::Begin("Debug Point Lights")) {
            int index {};  // TODO C++20

            for (PointLight& point_light : scene.point_lights) {
                ImGui::PushID(index++);
                ImGui::Text("Light %d", index);
                ImGui::SliderFloat3("Position", glm::value_ptr(point_light.position), -30.0f, 30.0f);
                ImGui::SliderFloat3("Ambient", glm::value_ptr(point_light.ambient_color), 0.0f, 1.0f);
                ImGui::SliderFloat3("Diffuse", glm::value_ptr(point_light.diffuse_color), 0.0f, 1.0f);
                ImGui::SliderFloat3("Specular", glm::value_ptr(point_light.specular_color), 0.0f, 1.0f);
                ImGui::SliderFloat("Falloff L", &point_light.falloff_linear, 0.0001f, 1.0f);
                ImGui::SliderFloat("Falloff Q", &point_light.falloff_quadratic, 0.00001f, 1.0f);
                ImGui::PopID();
                ImGui::Spacing();
            }
        }

        ImGui::End();
    }

    void DebugUi::draw_shadows_lines(
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
}
