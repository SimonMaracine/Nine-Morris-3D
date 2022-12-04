#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "nine_morris_3d_engine/application/window.h"
#include "nine_morris_3d_engine/graphics/imgui_context.h"

namespace imgui_context {
    void initialize(const std::unique_ptr<Window>& window) {
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init("#version 430 core");
        ImGui_ImplGlfw_InitForOpenGL(window->get_handle(), false);
    }

    void uninitialize() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void begin_frame() {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
    }

    void end_frame() {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
