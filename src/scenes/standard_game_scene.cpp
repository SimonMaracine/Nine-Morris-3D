#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "scenes/standard_game_scene.h"
#include "scenes/imgui_layer.h"
#include "game/nine_morris_3d.h"

void StandardGameScene::on_start() {
    imgui_layer.update();

    DEB_INFO("Entered game scene");
}

void StandardGameScene::on_stop() {
    imgui_layer.reset();
}

void StandardGameScene::on_awake() {
    imgui_layer = ImGuiLayer<StandardGameScene> {app, this};

    // It's ok to be called multiple times
    LOG_TOTAL_GPU_MEMORY_ALLOCATED();
}

void StandardGameScene::on_update() {

}

void StandardGameScene::on_fixed_update() {

}

void StandardGameScene::on_imgui_update() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app->data().width, app->data().height);
    io.DeltaTime = app->get_delta();

    imgui_layer.draw_menu_bar();

    if (imgui_layer.show_about) {
        imgui_layer.draw_about();
    } else if (imgui_layer.show_could_not_load_game) {
        imgui_layer.draw_could_not_load_game();
    } else if (imgui_layer.show_no_last_game) {
        imgui_layer.draw_no_last_game();
    } else if (board.get_phase() == BoardPhase::GameOver) {
        imgui_layer.draw_game_over();
    }

    if (undo_redo_state.undo.size() > 0) {
        imgui_layer.can_undo = true;
    }

    if (undo_redo_state.redo.size() > 0) {
        imgui_layer.can_redo = true;
    }

    if (imgui_layer.show_info && !imgui_layer.show_about) {
        ImGui::PushFont(app->user_data<Data>().imgui_info_font);
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::Text("FPS: %f", app->get_fps());
        ImGui::Text("OpenGL: %s", debug_opengl::get_opengl_version());
        ImGui::Text("GLSL: %s", debug_opengl::get_glsl_version());
        ImGui::Text("Vendor: %s", debug_opengl::get_vendor());
        ImGui::Text("Renderer: %s", debug_opengl::get_renderer());
        ImGui::End();
        ImGui::PopFont();
    }

#ifdef PLATFORM_GAME_DEBUG
    imgui_layer.draw_debug();
#endif
}
