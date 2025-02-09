#pragma once

namespace sm::internal {
    namespace imgui_context {
        // Initialize Dear ImGui and setup basic options
        void initialize(void* window, void* context);

        // Uninitialize Dear ImGui
        void uninitialize();

        // Called in the update portion for Dear ImGui
        void begin_frame();
        void end_frame();

        // Must be called for every SDL event in the window
        void process_event(const void* event);

        // Check if events should be propagated to the application or not
        bool want_capture_mouse();
        bool want_capture_keyboard();

        // Reset font texture
        void invalidate_font_texture();
    }
}
