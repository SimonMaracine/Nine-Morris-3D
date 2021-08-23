#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "application/layers/gui_layer.h"
#include "application/events.h"

void GuiLayer::on_attach() {
    imgui_start();

    game_layer = (GameLayer*) get_layer(0);
}

void GuiLayer::on_detach() {
    imgui_end();
}

void GuiLayer::on_update(float dt) {

}

void GuiLayer::on_draw() {

}

void GuiLayer::on_event(events::Event& event) {

}

bool GuiLayer::on_mouse_scrolled(events::MouseScrolledEvent& event) {

}

bool GuiLayer::on_mouse_moved(events::MouseMovedEvent& event) {

}

bool GuiLayer::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {

}

bool GuiLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {

}

bool GuiLayer::on_window_resized(events::WindowResizedEvent& event) {

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
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false)) {
                game_layer->restart();
            }
            if (ImGui::MenuItem("Undo", nullptr, false)) {
                systems::undo_move(game_layer->registry, game_layer->board);
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                application->running = false;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", nullptr, false)) {
                about = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (about) {
        ImGui::OpenPopup("About Nine Morris 3D");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
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
    }

    auto& state = game_layer->registry.get<GameStateComponent>(game_layer->board);

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
        }
    }

    ImGui::Begin("Debug");
    ImGui::Text("FPS: %f", application->fps);
    ImGui::Text("Frame time (ms): %f", dt * 1000.0f);
    if (ImGui::Button("VSync")) {
        static bool on = true;
        if (on) {
            application->window->set_vsync(0);
            on = false;
        } else {
            application->window->set_vsync(1);
            on = true;
        }
    }
    ImGui::Text("White pieces: %d", state.white_pieces_count);
    ImGui::Text("Black pieces: %d", state.black_pieces_count);
    ImGui::Text("Not placed pieces: %d", state.not_placed_pieces_count);
    ImGui::Text("Phase: %d", (int) state.phase);
    ImGui::Text("Turn: %s", state.turn == Player::White ? "white" : "black");
    ImGui::Text("Should take piece: %d", state.should_take_piece);
    ImGui::Text("Turns without mills: %d", state.turns_without_mills);
    ImGui::Text("History size (place): %lu", state.moves_history.placed_pieces.size());
    ImGui::Text("History size (move): %lu", state.moves_history.moved_pieces.size());
    ImGui::Text("History size (take): %lu", state.moves_history.taken_pieces.size());
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiLayer::imgui_end() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
