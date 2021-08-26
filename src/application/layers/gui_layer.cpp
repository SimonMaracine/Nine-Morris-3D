#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "application/layers/gui_layer.h"
#include "application/layers/game_layer.h"
#include "application/application.h"
#include "application/events.h"
#include "other/logging.h"

void GuiLayer::on_attach() {
    imgui_start();
    active = false;
}

void GuiLayer::on_detach() {
    imgui_end();
}

void GuiLayer::on_bind_layers() {
    game_layer = get_layer<GameLayer>(2);
}

void GuiLayer::on_update(float dt) {
    imgui_update(dt);
}

void GuiLayer::on_draw() {

}

void GuiLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(GuiLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(GuiLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(GuiLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(GuiLayer::on_mouse_button_released));
}

bool GuiLayer::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = event.scroll;

    if (hovering_gui)
        return true;
    else
        return false;
}

bool GuiLayer::on_mouse_moved(events::MouseMovedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event.mouse_x, event.mouse_y);

    if (hovering_gui)
        return true;
    else
        return false;
}

bool GuiLayer::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.button] = true;

    if (hovering_gui)
        return true;
    else
        return false;
}

bool GuiLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.button] = false;

    if (hovering_gui)
        return true;
    else
        return false;
}

bool GuiLayer::on_window_resized(events::WindowResizedEvent& event) {
    return false;
}

void GuiLayer::imgui_start() {
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImFontAtlas::GlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"ă");

    ImVector<ImWchar>* ranges = new ImVector<ImWchar>;
    builder.BuildRanges(ranges);

    io.FontDefault = io.Fonts->AddFontFromFileTTF("data/fonts/OpenSans-Semibold.ttf", 20.0f,
        nullptr, ranges->Data);

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = DEFAULT_BROWN;
    colors[ImGuiCol_TitleBgActive] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBg] = DEFAULT_BROWN;
    colors[ImGuiCol_Button] = DARK_BROWN;
    colors[ImGuiCol_ButtonHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_ButtonActive] = LIGHT_BROWN;
    colors[ImGuiCol_Header] = DARK_BROWN;
    colors[ImGuiCol_HeaderHovered] = DEFAULT_BROWN;

    ImGui_ImplOpenGL3_Init("#version 430 core");
    ImGui_ImplGlfw_InitForOpenGL(application->window->get_handle(), false);
}

void GuiLayer::imgui_update(float dt) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(application->data.width, application->data.height);
    io.DeltaTime = dt;

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    bool about = false;
    RESET_HOVERING_GUI;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false)) {
                game_layer->restart();
                // can_undo = false;
            }
            if (ImGui::MenuItem("Undo", nullptr, false)) {
                systems::undo(game_layer->registry, game_layer->board,
                              game_layer->storage, game_layer->assets);
                // can_undo = false;
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                application->running = false;
            }

            ImGui::EndMenu();
            HOVERING_GUI;
        }
        static bool vsync = true;
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("VSync", nullptr, &vsync)) {
                if (vsync) {
                    application->window->set_vsync(1);
                    SPDLOG_INFO("VSync enabled");
                } else {
                    application->window->set_vsync(0);
                    SPDLOG_INFO("VSync disabled");
                }
            }

            ImGui::EndMenu();
            HOVERING_GUI;
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", nullptr, false)) {
                about = true;
            }
            if (ImGui::MenuItem("Log Info", nullptr, false)) {
                logging::log_opengl_info(logging::LogTarget::File);
                SPDLOG_INFO("Logged OpenGL info");
            }

            ImGui::EndMenu();
            HOVERING_GUI;
        }

        ImGui::EndMainMenuBar();
    }

    if (about) {
        ImGui::OpenPopup("About Nine Morris 3D");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    } else {
        game_layer->active = true;
    }

    if (ImGui::BeginPopupModal("About Nine Morris 3D", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("A 3D implementation of the board game Nine Men's Morris");
        ImGui::Text("Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
        ImGui::Separator();
        ImGui::Text("All programming by:");
        ImGui::Text("Simon Teodor Mărăcine - simonmaracine@gmail.com");

        if (ImGui::Button("Ok", ImVec2(430, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
        HOVERING_GUI;
        game_layer->active = false;
    }

    auto& state = game_layer->registry.get<GameStateComponent>(game_layer->board);
    auto& moves_history = game_layer->registry.get<MovesHistoryComponent>(game_layer->board);

    if (state.phase == Phase::GameOver) {
        ImGui::OpenPopup("Game Over");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        if (ImGui::BeginPopupModal("Game Over", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            switch (state.ending) {
                case Ending::WinnerWhite: {
                    const char* message = "White player wins!";
                    float window_width = ImGui::GetWindowSize().x;
                    float text_width = ImGui::CalcTextSize(message).x;
                    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
                    ImGui::Text("%s", message);
                    break;
                }
                case Ending::WinnerBlack: {
                    const char* message = "Black player wins!";
                    float window_width = ImGui::GetWindowSize().x;
                    float text_width = ImGui::CalcTextSize(message).x;
                    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
                    ImGui::Text("%s", message);
                    break;
                }
                case Ending::TieBetweenBothPlayers: {
                    const char* message = "Tie between both players!";
                    float window_width = ImGui::GetWindowSize().x;
                    float text_width = ImGui::CalcTextSize(message).x;
                    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
                    ImGui::Text("%s", message);
                    break;
                }
                case Ending::None:
                    assert(false);
            }

            if (ImGui::Button("Ok", ImVec2(200, 0))) {
                ImGui::CloseCurrentPopup();
                state.phase = Phase::None;
            }

            ImGui::EndPopup();
            HOVERING_GUI;
        }
    }

    ImGui::Begin("Debug");
    ImGui::Text("FPS: %f", application->fps);
    ImGui::Text("Frame time (ms): %f", dt * 1000.0f);
    ImGui::Text("White pieces: %d", state.white_pieces_count);
    ImGui::Text("Black pieces: %d", state.black_pieces_count);
    ImGui::Text("Not placed pieces: %d", state.not_placed_pieces_count);
    ImGui::Text("Phase: %d", (int) state.phase);
    ImGui::Text("Turn: %s", state.turn == Player::White ? "white" : "black");
    ImGui::Text("Should take piece: %s", state.should_take_piece ? "true" : "false");
    ImGui::Text("Turns without mills: %d", state.turns_without_mills);
    ImGui::Text("History size (place): %lu", moves_history.placed_pieces.size());
    ImGui::Text("History size (move): %lu", moves_history.moved_pieces.size());
    ImGui::Text("History size (take): %lu", moves_history.taken_pieces.size());
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiLayer::imgui_end() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
