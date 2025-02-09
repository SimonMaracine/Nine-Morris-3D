#include "nine_morris_3d_engine/graphics/internal/imgui_context.hpp"

#include <cstring>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "nine_morris_3d_engine/application/platform.hpp"

namespace sm::internal {
    void imgui_context::initialize(void* window, void* context) {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(static_cast<SDL_Window*>(window), context);
        ImGui_ImplOpenGL3_Init("#version 430 core");

        [[maybe_unused]] ImGuiIO& io {ImGui::GetIO()};  // TODO other flags?
#ifdef SM_BUILD_DISTRIBUTION
        io.IniFilename = nullptr;
#endif
    }

    void imgui_context::uninitialize() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void imgui_context::begin_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void imgui_context::end_frame() {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void imgui_context::process_event(const void* event) {
        ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(event));
    }

    bool imgui_context::want_capture_mouse() {
        return ImGui::GetIO().WantCaptureMouse;
    }

    bool imgui_context::want_capture_keyboard() {
        return ImGui::GetIO().WantCaptureKeyboard;
    }

    void imgui_context::invalidate_font_texture() {
        ImGui_ImplOpenGL3_DestroyFontsTexture();
    }
}
