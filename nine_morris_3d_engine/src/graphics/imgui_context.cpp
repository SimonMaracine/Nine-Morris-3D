#include "engine/graphics/imgui_context.hpp"

#include <cstring>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include "engine/application_base/platform.hpp"

namespace sm {
    // Taken from Dear ImGui; it's not rock-solid, as it's just a workaround
    static int translate_untranslated_key(int key, int scancode) {
        if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL) {
            return key;
        }

        const char* key_name {glfwGetKeyName(key, scancode)};

        if (key_name && key_name[0] != 0 && key_name[1] == 0) {
            const char char_names[] {"`-=[]\\,;\'./"};
            const int char_keys[] {
                GLFW_KEY_GRAVE_ACCENT,
                GLFW_KEY_MINUS,
                GLFW_KEY_EQUAL,
                GLFW_KEY_LEFT_BRACKET,
                GLFW_KEY_RIGHT_BRACKET,
                GLFW_KEY_BACKSLASH,
                GLFW_KEY_COMMA,
                GLFW_KEY_SEMICOLON,
                GLFW_KEY_APOSTROPHE,
                GLFW_KEY_PERIOD,
                GLFW_KEY_SLASH,
                0
            };

            if (key_name[0] >= '0' && key_name[0] <= '9') {
                key = GLFW_KEY_0 + (key_name[0] - '0');
            } else if (key_name[0] >= 'A' && key_name[0] <= 'Z') {
                key = GLFW_KEY_A + (key_name[0] - 'A');
            } else if (key_name[0] >= 'a' && key_name[0] <= 'z') {
                key = GLFW_KEY_A + (key_name[0] - 'a');
            } else if (const char* p {std::strchr(char_names, key_name[0])}) {
                key = char_keys[p - char_names];
            }
        }

        return key;
    }

    // Taken from Dear ImGui
    static ImGuiKey glfw_to_imgui_key(int key) {
        switch (key) {
            case GLFW_KEY_TAB: return ImGuiKey_Tab;
            case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
            case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
            case GLFW_KEY_UP: return ImGuiKey_UpArrow;
            case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
            case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
            case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
            case GLFW_KEY_HOME: return ImGuiKey_Home;
            case GLFW_KEY_END: return ImGuiKey_End;
            case GLFW_KEY_INSERT: return ImGuiKey_Insert;
            case GLFW_KEY_DELETE: return ImGuiKey_Delete;
            case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
            case GLFW_KEY_SPACE: return ImGuiKey_Space;
            case GLFW_KEY_ENTER: return ImGuiKey_Enter;
            case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
            case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
            case GLFW_KEY_COMMA: return ImGuiKey_Comma;
            case GLFW_KEY_MINUS: return ImGuiKey_Minus;
            case GLFW_KEY_PERIOD: return ImGuiKey_Period;
            case GLFW_KEY_SLASH: return ImGuiKey_Slash;
            case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
            case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
            case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
            case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
            case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
            case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
            case GLFW_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
            case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
            case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
            case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
            case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
            case GLFW_KEY_KP_0: return ImGuiKey_Keypad0;
            case GLFW_KEY_KP_1: return ImGuiKey_Keypad1;
            case GLFW_KEY_KP_2: return ImGuiKey_Keypad2;
            case GLFW_KEY_KP_3: return ImGuiKey_Keypad3;
            case GLFW_KEY_KP_4: return ImGuiKey_Keypad4;
            case GLFW_KEY_KP_5: return ImGuiKey_Keypad5;
            case GLFW_KEY_KP_6: return ImGuiKey_Keypad6;
            case GLFW_KEY_KP_7: return ImGuiKey_Keypad7;
            case GLFW_KEY_KP_8: return ImGuiKey_Keypad8;
            case GLFW_KEY_KP_9: return ImGuiKey_Keypad9;
            case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
            case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
            case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
            case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
            case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
            case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
            case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
            case GLFW_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
            case GLFW_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
            case GLFW_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
            case GLFW_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
            case GLFW_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
            case GLFW_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
            case GLFW_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
            case GLFW_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
            case GLFW_KEY_MENU: return ImGuiKey_Menu;
            case GLFW_KEY_0: return ImGuiKey_0;
            case GLFW_KEY_1: return ImGuiKey_1;
            case GLFW_KEY_2: return ImGuiKey_2;
            case GLFW_KEY_3: return ImGuiKey_3;
            case GLFW_KEY_4: return ImGuiKey_4;
            case GLFW_KEY_5: return ImGuiKey_5;
            case GLFW_KEY_6: return ImGuiKey_6;
            case GLFW_KEY_7: return ImGuiKey_7;
            case GLFW_KEY_8: return ImGuiKey_8;
            case GLFW_KEY_9: return ImGuiKey_9;
            case GLFW_KEY_A: return ImGuiKey_A;
            case GLFW_KEY_B: return ImGuiKey_B;
            case GLFW_KEY_C: return ImGuiKey_C;
            case GLFW_KEY_D: return ImGuiKey_D;
            case GLFW_KEY_E: return ImGuiKey_E;
            case GLFW_KEY_F: return ImGuiKey_F;
            case GLFW_KEY_G: return ImGuiKey_G;
            case GLFW_KEY_H: return ImGuiKey_H;
            case GLFW_KEY_I: return ImGuiKey_I;
            case GLFW_KEY_J: return ImGuiKey_J;
            case GLFW_KEY_K: return ImGuiKey_K;
            case GLFW_KEY_L: return ImGuiKey_L;
            case GLFW_KEY_M: return ImGuiKey_M;
            case GLFW_KEY_N: return ImGuiKey_N;
            case GLFW_KEY_O: return ImGuiKey_O;
            case GLFW_KEY_P: return ImGuiKey_P;
            case GLFW_KEY_Q: return ImGuiKey_Q;
            case GLFW_KEY_R: return ImGuiKey_R;
            case GLFW_KEY_S: return ImGuiKey_S;
            case GLFW_KEY_T: return ImGuiKey_T;
            case GLFW_KEY_U: return ImGuiKey_U;
            case GLFW_KEY_V: return ImGuiKey_V;
            case GLFW_KEY_W: return ImGuiKey_W;
            case GLFW_KEY_X: return ImGuiKey_X;
            case GLFW_KEY_Y: return ImGuiKey_Y;
            case GLFW_KEY_Z: return ImGuiKey_Z;
            case GLFW_KEY_F1: return ImGuiKey_F1;
            case GLFW_KEY_F2: return ImGuiKey_F2;
            case GLFW_KEY_F3: return ImGuiKey_F3;
            case GLFW_KEY_F4: return ImGuiKey_F4;
            case GLFW_KEY_F5: return ImGuiKey_F5;
            case GLFW_KEY_F6: return ImGuiKey_F6;
            case GLFW_KEY_F7: return ImGuiKey_F7;
            case GLFW_KEY_F8: return ImGuiKey_F8;
            case GLFW_KEY_F9: return ImGuiKey_F9;
            case GLFW_KEY_F10: return ImGuiKey_F10;
            case GLFW_KEY_F11: return ImGuiKey_F11;
            case GLFW_KEY_F12: return ImGuiKey_F12;
            default: return ImGuiKey_None;
        }
    }

    void imgui_context::initialize(GLFWwindow* window_handle) {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window_handle, false);
        ImGui_ImplOpenGL3_Init("#version 430 core");

        [[maybe_unused]] ImGuiIO& io {ImGui::GetIO()};  // TODO other flags?
#ifdef SM_BUILD_DISTRIBUTION
        io.IniFilename = nullptr;
#endif
    }

    void imgui_context::uninitialize() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void imgui_context::begin_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void imgui_context::end_frame() {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool imgui_context::on_mouse_wheel_scrolled(float yoffset) {
        ImGuiIO& io {ImGui::GetIO()};
        io.AddMouseWheelEvent(0.0f, yoffset);

        return io.WantCaptureMouse;
    }

    bool imgui_context::on_mouse_moved(float xpos, float ypos) {
        ImGuiIO& io {ImGui::GetIO()};
        io.AddMousePosEvent(xpos, ypos);

        return io.WantCaptureMouse;
    }

    bool imgui_context::on_mouse_button_pressed(int button) {
        ImGuiIO& io {ImGui::GetIO()};
        io.AddMouseButtonEvent(button, true);

        return io.WantCaptureMouse;
    }

    bool imgui_context::on_mouse_button_released(int button) {
        ImGuiIO& io {ImGui::GetIO()};
        io.AddMouseButtonEvent(button, false);

        return io.WantCaptureMouse;
    }

    bool imgui_context::on_key_pressed(int key, int scancode) {
        key = translate_untranslated_key(key, scancode);

        ImGuiIO& io {ImGui::GetIO()};
        io.AddKeyEvent(glfw_to_imgui_key(key), true);

        return io.WantCaptureKeyboard;
    }

    bool imgui_context::on_key_released(int key, int scancode) {
        key = translate_untranslated_key(key, scancode);

        ImGuiIO& io {ImGui::GetIO()};
        io.AddKeyEvent(glfw_to_imgui_key(key), false);

        return io.WantCaptureKeyboard;
    }

    bool imgui_context::on_char_typed(unsigned int codepoint) {
        ImGuiIO& io {ImGui::GetIO()};
        io.AddInputCharacter(codepoint);

        return io.WantCaptureKeyboard;
    }
}
