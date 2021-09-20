#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "application/events.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/framebuffer.h"
#include "ecs/systems.h"
#include "other/logging.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/save_load.h"

void ImGuiLayer::on_attach() {
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"ă");

    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);

    io.FontDefault = io.Fonts->AddFontFromFileTTF("data/fonts/OpenSans-Semibold.ttf", 20.0f,
            nullptr, ranges.Data);
    io.Fonts->Build();

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
    ImGui_ImplGlfw_InitForOpenGL(app->window->get_handle(), false);
}

void ImGuiLayer::on_detach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::on_bind_layers() {
    game_layer = get_layer<GameLayer>(0, scene);
    gui_layer = get_layer<GuiLayer>(1, scene);
}

void ImGuiLayer::on_update(float dt) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app->data.width, app->data.height);
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
                can_undo = false;
            }
            if (ImGui::MenuItem("Load Last", nullptr, false)) {
                game_layer->load_game();
            }
            if (ImGui::MenuItem("Save", nullptr, false)) {
                save_load::save_game(scene->registry, save_load::gather_entities(scene->board,
                    scene->camera, scene->nodes, scene->pieces));
            }
            if (ImGui::MenuItem("Undo", nullptr, false, can_undo)) {
                systems::undo(scene->registry, scene->board);

                auto& state = scene->registry.get<GameStateComponent>(scene->board);

                if (state.not_placed_pieces_count == 18) {
                    can_undo = false;
                }
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                app->running = false;
            }

            ImGui::EndMenu();
            HOVERING_GUI;
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("VSync", nullptr, &scene->options.vsync)) {
                if (scene->options.vsync) {
                    app->window->set_vsync(1);

                    SPDLOG_INFO("VSync enabled");
                } else {
                    app->window->set_vsync(0);

                    SPDLOG_INFO("VSync disabled");
                }
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &scene->options.save_on_exit)) {
                if (scene->options.save_on_exit) {
                    SPDLOG_INFO("The game will be saved on exit");
                } else {
                    SPDLOG_INFO("The game will not be saved on exit");
                }
            }
            if (ImGui::BeginMenu("Anti-Aliasing", true)) {
                if (ImGui::RadioButton("No Anti-Aliasing", &scene->options.samples, 1)) {
                    game_layer->set_scene_framebuffer(scene->options.samples);

                    SPDLOG_INFO("Anti-aliasing disabled");
                }
                if (ImGui::RadioButton("2x", &scene->options.samples, 2)) {
                    game_layer->set_scene_framebuffer(scene->options.samples);

                    SPDLOG_INFO("2x anti-aliasing");
                }
                if (ImGui::RadioButton("4x", &scene->options.samples, 4)) {
                    game_layer->set_scene_framebuffer(scene->options.samples);

                    SPDLOG_INFO("4x anti-aliasing");
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Texture Quality", true)) {
                if (ImGui::RadioButton("High", &scene->options.texture_quality, 0)) {
                    game_layer->set_textures_quality(scene->options.texture_quality);

                    SPDLOG_INFO("Textures set to high quality");
                }
                if (ImGui::RadioButton("Normal", &scene->options.texture_quality, 1)) {
                    game_layer->set_textures_quality(scene->options.texture_quality);

                    SPDLOG_INFO("Textures set to normal quality");
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
            HOVERING_GUI;
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", nullptr, false)) {
                about = true;
            }
            if (ImGui::MenuItem("Log Info", nullptr, false)) {
                logging::log_opengl_and_dependencies_info(logging::LogTarget::File);

                SPDLOG_INFO("Logged OpenGL and dependencies info");
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
        gui_layer->active = true;
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
        gui_layer->active = false;
    }

    auto& state = scene->registry.get<GameStateComponent>(scene->board);

    if (state.not_placed_pieces_count < 18) {
        can_undo = true;
    }

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

#ifndef NDEBUG
    auto& moves_history = scene->registry.get<MovesHistoryComponent>(scene->board);

    ImGui::Begin("Debug");
    ImGui::Text("FPS: %f", app->fps);
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
#endif

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::on_draw() {

}

void ImGuiLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(ImGuiLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(ImGuiLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(ImGuiLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(ImGuiLayer::on_mouse_button_released));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(ImGuiLayer::on_window_resized));
}

bool ImGuiLayer::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = event.scroll;

    if (hovering_gui)
        return true;
    else
        return false;
}

bool ImGuiLayer::on_mouse_moved(events::MouseMovedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event.mouse_x, event.mouse_y);

    if (hovering_gui)
        return true;
    else
        return false;
}

bool ImGuiLayer::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.button] = true;

    if (hovering_gui)
        return true;
    else
        return false;
}

bool ImGuiLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.button] = false;

    if (hovering_gui)
        return true;
    else
        return false;
}

bool ImGuiLayer::on_window_resized(events::WindowResizedEvent& event) {
    app->storage->scene_framebuffer->resize(event.width, event.height);
    app->storage->intermediate_framebuffer->resize(event.width, event.height);
    systems::projection_matrix(scene->registry, (float) event.width, (float) event.height);
    app->storage->orthographic_projection_matrix = glm::ortho(0.0f, (float) event.width, 0.0f, (float) event.height);

    return false;
}
