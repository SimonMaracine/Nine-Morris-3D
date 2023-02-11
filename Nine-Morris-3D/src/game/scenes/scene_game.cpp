#include <engine/engine_application.h>
#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>
#include <engine/engine_scene.h>

#include "game/entities/boards/standard_board.h"
#include "game/entities/boards/jump_board.h"
#include "game/scenes/scene_game.h"
#include "game/save_load.h"
#include "game/game_options.h"
#include "other/constants.h"
#include "other/data.h"

#define RESET_HOVERING_GUI() hovering_gui = false;
#define HOVERING_GUI() hovering_gui = true;

void SceneGame::setup_and_add_model_board() {
    auto& board = get_board();

    board.model->scale = WORLD_SCALE;
    board.model->vertex_array = app->res.vertex_array["board_wood"_H];
    board.model->index_buffer = app->res.index_buffer["board_wood"_H];
    board.model->material = app->res.material_instance["board_wood"_H];
    board.model->cast_shadow = true;
    board.model->bounding_box = std::make_optional<renderables::Model::BoundingBox>();
    board.model->bounding_box->id = identifier::null;
    board.model->bounding_box->size = BOARD_BOUNDING_BOX;
    board.model->bounding_box->sort = false;

    scene_list.add(get_board().model);

    DEB_DEBUG("Setup model board");
}

void SceneGame::setup_and_add_model_board_paint() {
    const auto& board = get_board();

    board.paint_model->position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model->scale = WORLD_SCALE;
    board.paint_model->vertex_array = app->res.vertex_array["board_paint"_H];
    board.paint_model->index_buffer = app->res.index_buffer["board_paint"_H];
    board.paint_model->material = app->res.material_instance["board_paint"_H];

    scene_list.add(get_board().paint_model);

    DEB_DEBUG("Setup model board paint");
}

void SceneGame::setup_and_add_model_piece(size_t index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    const Piece& piece = get_board().pieces.at(index);

    const auto id = piece.type == PieceType::White ? "white_piece"_H : "black_piece"_H;

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->scale = WORLD_SCALE;
    piece.model->vertex_array = app->res.vertex_array[id];
    piece.model->index_buffer = app->res.index_buffer[id];
    piece.model->material = app->res.material_instance[hs("piece" + std::to_string(index))];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->cast_shadow = true;
    piece.model->bounding_box = std::make_optional<renderables::Model::BoundingBox>();
    piece.model->bounding_box->id = data.piece_ids[index];
    piece.model->bounding_box->size = PIECE_BOUNDING_BOX;

    scene_list.add(piece.model);

    piece.source->set_position(position);
    piece.source->set_reference_distance(6.0f);

    DEB_DEBUG("Setup model piece {}", index);
}

void SceneGame::setup_and_add_model_nodes() {
    for (size_t i = 0; i < MAX_NODES; i++) {
        setup_and_add_model_node(i, NODE_POSITIONS[i]);
    }
}

void SceneGame::setup_and_add_model_node(size_t index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    const Node& node = get_board().nodes.at(index);

    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->vertex_array = app->res.vertex_array["node"_H];
    node.model->index_buffer = app->res.index_buffer["node"_H];
    node.model->material = app->res.material_instance[hs("node" + std::to_string(index))];
    node.model->bounding_box = std::make_optional<renderables::Model::BoundingBox>();
    node.model->bounding_box->id = data.node_ids[index];
    node.model->bounding_box->size = NODE_BOUNDING_BOX;

    scene_list.add(node.model);

    DEB_DEBUG("Setup model node {}", index);
}


void SceneGame::setup_piece_on_node(size_t index, size_t node_index) {
    setup_and_add_model_piece(index, PIECE_INDEX_POSITION(node_index));

    get_board().pieces.at(index).node_index = node_index;
    get_board().nodes.at(node_index).piece_index = index;
}

void SceneGame::setup_camera() {
    auto& data = app->user_data<Data>();

    static constexpr float PITCH = 47.0f;
    static constexpr float DISTANCE_TO_POINT = 8.0f;

    camera = Camera {};

    camera_controller = PointCameraController {
        &camera,
        app->data().width,
        app->data().height,
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT,
        PITCH,
        data.options.sensitivity
    };

    default_camera_position = camera_controller.get_position();

    camera_controller = PointCameraController {
        &camera,
        app->data().width,
        app->data().height,
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT + 0.7f,
        PITCH,
        data.options.sensitivity
    };

    app->renderer->set_camera_controller(&camera_controller);
    update_listener();

    DEB_DEBUG("Setup camera");
}

void SceneGame::setup_and_add_turn_indicator() {
    auto turn_indicator = objects.add<gui::Image>(
        "turn_indicator"_H, app->res.texture["white_indicator"_H]
    );

    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right);
    turn_indicator->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);

    scene_list.add(turn_indicator);
}

void SceneGame::setup_and_add_timer_text() {
    auto& data = app->user_data<Data>();

    auto timer_text = objects.add<gui::Text>(
        "timer_text"_H,
        app->res.font["open_sans"_H],
        "00:00",
        1.5f,
        glm::vec3(0.8f)
    );

    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        scene_list.add(timer_text);
    }
}

void SceneGame::setup_wait_indicator() {
    auto wait_indicator = objects.add<gui::Image>(
        "wait_indicator"_H, app->res.texture["wait_indicator"_H]
    );

    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right);
    wait_indicator->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

void SceneGame::setup_computer_thinking_indicator() {
    auto computer_thinking_indicator = objects.add<gui::Image>(
        "computer_thinking_indicator"_H, app->res.texture["computer_thinking_indicator"_H]
    );

    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right);
    computer_thinking_indicator->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

void SceneGame::setup_light_bulb() {
    auto light_bulb = objects.add<renderables::Quad>("light_bulb"_H);

    light_bulb->texture = app->res.texture["light_bulb"_H];

#ifdef NM3D_PLATFORM_DEBUG
    auto& data = app->user_data<Data>();

    if (data.options.skybox == game_options::FIELD) {
        light_bulb->position = LIGHT_FIELD.position;
    } else if (data.options.skybox == game_options::AUTUMN) {
        light_bulb->position = LIGHT_AUTUMN.position;
    } else if (data.options.skybox == game_options::NONE) {
        light_bulb->position = LIGHT_NONE.position;
    }
#endif
}

void SceneGame::initialize_piece(size_t index, std::shared_ptr<gl::Texture> diffuse_texture) {
    auto material_instance = app->res.material_instance.load(
        hs("piece" + std::to_string(index)),
        app->res.material["tinted_wood"_H]
    );

    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_texture("u_material.normal"_H, app->res.texture["piece_normal"_H], 1);
    material_instance->set_vec3("u_material.tint"_H, DEFAULT_TINT);
}

void SceneGame::initialize_piece_no_normal(size_t index, std::shared_ptr<gl::Texture> diffuse_texture) {
    auto material_instance = app->res.material_instance.load(
        hs("piece" + std::to_string(index)),
        app->res.material["tinted_wood"_H]
    );

    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_vec3("u_material.tint"_H, DEFAULT_TINT);
}

void SceneGame::release_piece_material_instances() {
    for (size_t i = 0; i < MAX_PIECES; i++) {
        app->res.material_instance.release(hs("piece" + std::to_string(i)));
    }
}

void SceneGame::change_skybox() {
    auto& data = app->user_data<Data>();

    const std::array<std::shared_ptr<TextureData>, 6> texture_data = {
        app->res.texture_data["skybox_px"_H],
        app->res.texture_data["skybox_nx"_H],
        app->res.texture_data["skybox_py"_H],
        app->res.texture_data["skybox_ny"_H],
        app->res.texture_data["skybox_pz"_H],
        app->res.texture_data["skybox_nz"_H]
    };

    auto texture = app->res.texture_3d.force_load("skybox"_H, texture_data);
    app->renderer->set_skybox(texture);

    if (data.options.skybox == game_options::FIELD) {  // FIXME this is not dry
        app->renderer->directional_light = LIGHT_FIELD;
        app->renderer->light_space = SHADOWS_FIELD;
    } else if (data.options.skybox == game_options::AUTUMN) {
        app->renderer->directional_light = LIGHT_AUTUMN;
        app->renderer->light_space = SHADOWS_AUTUMN;
    } else if (data.options.skybox == game_options::NONE) {
        app->renderer->directional_light = LIGHT_NONE;
        app->renderer->light_space = SHADOWS_NONE;
    } else {
        ASSERT(false, "Invalid skybox");
    }

    app->res.texture_data.release("skybox"_H);
}

void SceneGame::change_board_paint_texture() {
    auto& data = app->user_data<Data>();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = app->res.texture.force_load(
        "board_paint_diffuse"_H,
        app->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    app->res.material_instance["board_paint"_H]->set_texture("u_material.diffuse"_H, diffuse_texture, 0);

    app->res.texture_data.release("board_paint_diffuse"_H);
}

void SceneGame::update_listener() {
    auto& listener = app->openal->get_listener();

    listener.set_position(camera_controller.get_position());

    listener.set_look_at_and_up(
        camera_controller.get_point() - camera_controller.get_position(),
        glm::rotate(UP_VECTOR, camera_controller.get_rotation().y, UP_VECTOR)
    );
}

void SceneGame::update_cursor() {
    auto& data = app->user_data<Data>();

    if (data.options.custom_cursor) {
        if (get_board().must_take_piece) {
            app->window->set_cursor(data.cross_cursor);

            objects.get<renderables::Quad>("keyboard_controls"_H)->texture = app->res.texture["keyboard_controls_cross"_H];
        } else {
            app->window->set_cursor(data.arrow_cursor);

            objects.get<renderables::Quad>("keyboard_controls"_H)->texture = app->res.texture["keyboard_controls_default"_H];
        }
    }
}

void SceneGame::update_turn_indicator() {
    if (get_board().turn == BoardPlayer::White) {
        objects.get<gui::Image>("turn_indicator"_H)->set_image(app->res.texture["white_indicator"_H]);
    } else {
        objects.get<gui::Image>("turn_indicator"_H)->set_image(app->res.texture["black_indicator"_H]);
    }
}

void SceneGame::update_wait_indicator() {
    if (!get_board().next_move) {
        if (!show_wait_indicator) {
            scene_list.add(objects.get<gui::Image>("wait_indicator"_H));
            show_wait_indicator = true;
        }
    } else {
        if (show_wait_indicator) {
            scene_list.remove(objects.get<gui::Image>("wait_indicator"_H));
            show_wait_indicator = false;
        }
    }
}

void SceneGame::update_computer_thinking_indicator() {
    if (game.state == GameState::ComputerThinkingMove) {
        if (!show_computer_thinking_indicator) {
            scene_list.add(objects.get<gui::Image>("computer_thinking_indicator"_H));
            show_computer_thinking_indicator = true;
        }
    } else {
        if (show_computer_thinking_indicator) {
            scene_list.remove(objects.get<gui::Image>("computer_thinking_indicator"_H));
            show_computer_thinking_indicator = false;
        }
    }
}

void SceneGame::update_timer_text() {
    const auto time = timer.get_time_formatted();
    objects.get<gui::Text>("timer_text"_H)->set_text(time);
}

void SceneGame::update_after_human_move(bool did_action, bool switched_turn, bool must_take_or_took_piece) {
    if (did_action) {
        if (switched_turn) {
            game.state = GameState::HumanDoingMove;
        } else {
            game.state = GameState::HumanDoingMoveAndTake;
        }
    }

    if (did_action && !made_first_move && !timer.is_running()) {
        timer.start();
        made_first_move = true;
    }

    if (get_board().phase == BoardPhase::GameOver) {
        timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator();
    }

    if (must_take_or_took_piece) {
        update_cursor();
    }

    can_undo = get_undo_size() > 0;
    can_redo = get_redo_size() > 0;
}

void SceneGame::update_after_computer_move(bool switched_turn) {
    if (!made_first_move && !timer.is_running()) {
        timer.start();
        made_first_move = true;
    }

    if (get_board().phase == BoardPhase::GameOver) {
        timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator();
    }

    can_undo = get_undo_size() > 0;
    can_redo = get_redo_size() > 0;
}

void SceneGame::update_game_state() {
    switch (game.state) {
        case GameState::NextPlayer:
            if (get_board().turn == BoardPlayer::White) {
                switch (game.white_player) {
                    case GamePlayer::None:
                        ASSERT(false, "Player must not be None");
                        break;
                    case GamePlayer::Human:
                        game.state = GameState::HumanBeginMove;
                        break;
                    case GamePlayer::Computer:
                        game.state = GameState::ComputerBeginMove;
                        break;
                }
            } else {
                switch (game.black_player) {
                    case GamePlayer::None:
                        ASSERT(false, "Player must not be None");
                        break;
                    case GamePlayer::Human:
                        game.state = GameState::HumanBeginMove;
                        break;
                    case GamePlayer::Computer:
                        game.state = GameState::ComputerBeginMove;
                        break;
                }
            }
            break;
        case GameState::HumanBeginMove:
            game.human_begin_move();
            game.state = GameState::HumanThinkingMove;
            break;
        case GameState::HumanThinkingMove:
            break;
        case GameState::HumanDoingMove:
            if (get_board().next_move) {
                game.state = GameState::HumanEndMove;
            }
            break;
        case GameState::HumanDoingMoveAndTake:
            if (get_board().next_move) {
                game.state = GameState::HumanThinkingMove;
            }
            break;
        case GameState::HumanEndMove:
            game.state = GameState::NextPlayer;
            break;
        case GameState::ComputerBeginMove:
            game.computer_think_move();
            game.state = GameState::ComputerThinkingMove;
            break;
        case GameState::ComputerThinkingMove:
            if (!minimax_thread.is_running()) {
                minimax_thread.join();

                const bool switched_turn = game.computer_execute_move();

                update_after_computer_move(switched_turn);

                if (switched_turn) {
                    game.state = GameState::ComputerDoingMove;
                } else {
                    game.state = GameState::ComputerDoingMoveAndTake;
                }
            }
            break;
        case GameState::ComputerDoingMove:
            if (get_board().next_move) {
                game.state = GameState::ComputerEndMove;
            }
            break;
        case GameState::ComputerDoingMoveAndTake:
            if (get_board().next_move) {
                game.computer_execute_take_move();
                game.state = GameState::ComputerDoingMove;
            }
            break;
        case GameState::ComputerEndMove:
            game.state = GameState::NextPlayer;
            break;
    }
}

void SceneGame::update_all_imgui() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app->data().width, app->data().height);
    io.DeltaTime = app->get_delta();

    if (get_board().phase == BoardPhase::GameOver && get_board().next_move) {
        window = WindowImGui::ShowGameOver;
    }

    imgui_draw_menu_bar();

    switch (window) {
        case WindowImGui::None:
            // Do nothing
            break;
        case WindowImGui::ShowAbout:
            imgui_draw_about();
            break;
        case WindowImGui::ShowCouldNotLoadGame:
            imgui_draw_could_not_load_game();
            break;
        case WindowImGui::ShowNoLastGame:
            imgui_draw_no_last_game();
            break;
        case WindowImGui::ShowGameOver:
            imgui_draw_game_over();
            break;
        case WindowImGui::ShowRulesStandardGame:
            imgui_draw_rules_standard_game();
            break;
        case WindowImGui::ShowRulesJumpVariant:
            imgui_draw_rules_jump_variant();
            break;
        case WindowImGui::ShowRulesJumpPlusVariant:
            imgui_draw_rules_jump_plus_variant();
            break;
    }

    if (show_info) {
        imgui_draw_info();
    }

#ifdef NM3D_PLATFORM_DEBUG
    imgui_draw_debug();
#endif
}

void SceneGame::set_skybox(Skybox skybox) {
    if (skybox == Skybox::None) {
        app->renderer->set_skybox(nullptr);
        return;
    }

    auto& data = app->user_data<Data>();

    skybox_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.skybox);
}

void SceneGame::set_board_paint_texture() {
    auto& data = app->user_data<Data>();

    board_paint_texture_loader->start_loading_thread(
        data.launcher_options.texture_quality,
        data.options.labeled_board
    );
}

void SceneGame::imgui_initialize() {
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#ifdef NM3D_PLATFORM_RELEASE
    io.IniFilename = nullptr;
#endif
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = DEFAULT_BROWN;
    colors[ImGuiCol_TitleBgActive] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBg] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBgHovered] = DARK_BROWN;
    colors[ImGuiCol_FrameBgActive] = LIGHT_BROWN;
    colors[ImGuiCol_Button] = DARK_BROWN;
    colors[ImGuiCol_ButtonHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_ButtonActive] = LIGHT_BROWN;
    colors[ImGuiCol_Header] = DARK_BROWN;
    colors[ImGuiCol_HeaderHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_HeaderActive] = LIGHT_BROWN;
    colors[ImGuiCol_CheckMark] = BEIGE;
    colors[ImGuiCol_SliderGrab] = LIGHT_GRAY_BLUE;
    colors[ImGuiCol_SliderGrabActive] = LIGHT_GRAY_BLUE;
    colors[ImGuiCol_Tab] = DARK_BROWN;
    colors[ImGuiCol_TabHovered] = LIGHT_BROWN;
    colors[ImGuiCol_TabActive] = DEFAULT_BROWN;
    colors[ImGuiCol_Text] = GRAYISH_TEXT;
    colors[ImGuiCol_WindowBg] = BLACK_BACKGROUND;
    colors[ImGuiCol_ChildBg] = BLACK_BACKGROUND;
    colors[ImGuiCol_PopupBg] = BLACK_BACKGROUND;
    colors[ImGuiCol_MenuBarBg] = BLACK_BACKGROUND;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.FrameRounding = 4;
    style.WindowRounding = 6;
    style.ChildRounding = 6;
    style.PopupRounding = 6;
    style.GrabRounding = 6;
    style.GrabMinSize = 12;
    style.FramePadding = ImVec2(5.0f, 4.0f);

    imgui_initialize_options();  // FIXME not needed
}

void SceneGame::imgui_reset() {
    hovering_gui = false;
    can_undo = false;
    can_redo = false;
    show_info = false;
    window = WindowImGui::None;
}

void SceneGame::imgui_draw_menu_bar() {
    RESET_HOVERING_GUI();

    auto& data = app->user_data<Data>();

    if (ImGui::BeginMainMenuBar()) {
        const bool can_change = game.state == GameState::HumanThinkingMove;
        const bool can_undo_redo = (
            game.state == GameState::HumanThinkingMove
            || game.state == GameState::ComputerThinkingMove
        );

        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false, can_change)) {
                app->change_scene(app->get_current_scene()->get_id());

                DEB_INFO("Restarted current game");
            }
            if (ImGui::MenuItem("Load Last Game", nullptr, false, can_change)) {
                if (last_save_game_date == save_load::NO_LAST_GAME) {
                    window = WindowImGui::ShowNoLastGame;
                } else {
                    load_game();
                }
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", last_save_game_date.c_str());
            }
            if (ImGui::MenuItem("Save Game", nullptr, false, can_change)) {
                save_game();

                time_t current;  // TODO this time might be quite later
                time(&current);
                last_save_game_date = ctime(&current);
            }
            if (ImGui::BeginMenu("Game Mode", can_change)) {
                if (ImGui::RadioButton("Standard Game", &data.imgui_option.scene, game_options::STANDARD)) {
                    if (data.imgui_option.scene != data.options.scene) {
                        data.options.scene = data.imgui_option.scene;
                        app->change_scene("standard_game"_H);

                        DEB_INFO("Changed scene to standard game");
                    }
                }
                if (ImGui::RadioButton("Jump Variant", &data.imgui_option.scene, game_options::JUMP)) {
                    if (data.imgui_option.scene != data.options.scene) {
                        data.options.scene = data.imgui_option.scene;
                        app->change_scene("jump_variant"_H);

                        DEB_INFO("Changed scene to jump variant");
                    }
                }
                if (ImGui::RadioButton("Jump Plus Variant", &data.imgui_option.scene, game_options::JUMP_PLUS)) {
                    if (data.imgui_option.scene != data.options.scene) {
                        data.options.scene = data.imgui_option.scene;
                        app->change_scene("jump_plus_variant"_H);

                        DEB_INFO("Changed scene to jump plus variant");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::BeginMenu("Players", can_change)) {
                if (ImGui::BeginMenu("White")) {
                    if (ImGui::RadioButton("Human", &data.options.white_player, game_options::HUMAN)) {
                        game.white_player = GamePlayer::Human;
                        game.reset_players();

                        DEB_INFO("Set white player to human");
                    }
                    if (ImGui::RadioButton("Computer", &data.options.white_player, game_options::COMPUTER)) {
                        game.white_player = GamePlayer::Computer;
                        game.reset_players();

                        DEB_INFO("Set white player to computer");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }
                if (ImGui::BeginMenu("Black")) {
                    if (ImGui::RadioButton("Human", &data.options.black_player, game_options::HUMAN)) {
                        game.black_player = GamePlayer::Human;
                        game.reset_players();

                        DEB_INFO("Set black player to human");
                    }
                    if (ImGui::RadioButton("Computer", &data.options.black_player, game_options::COMPUTER)) {
                        game.black_player = GamePlayer::Computer;
                        game.reset_players();

                        DEB_INFO("Set black player to computer");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Undo", nullptr, false, can_undo && can_undo_redo)) {
                undo();

                can_undo = get_undo_size() > 0;
                can_redo = get_redo_size() > 0;
            }
            if (ImGui::MenuItem("Redo", nullptr, false, can_redo && can_undo_redo)) {
                redo();

                can_undo = get_undo_size() > 0;
                can_redo = get_redo_size() > 0;
            }
            if (ImGui::MenuItem("Exit To Launcher", nullptr, false)) {
                app->running = false;
                app->exit_code = 1;
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                app->running = false;
            }

            ImGui::EndMenu();
            HOVERING_GUI()
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Graphics")) {
                if (ImGui::MenuItem("VSync", nullptr, &data.options.vsync)) {
                    if (data.options.vsync) {
                        app->window->set_vsync(data.options.vsync);

                        DEB_INFO("VSync enabled");
                    } else {
                        app->window->set_vsync(data.options.vsync);

                        DEB_INFO("VSync disabled");
                    }
                }
                if (ImGui::MenuItem("Custom Cursor", nullptr, &data.options.custom_cursor)) {
                    if (data.options.custom_cursor) {
                        if (get_board().must_take_piece) {
                            app->window->set_cursor(data.cross_cursor);
                        } else {
                            app->window->set_cursor(data.arrow_cursor);
                        }

                        DEB_INFO("Set custom cursor");
                    } else {
                        app->window->set_cursor(0);

                        DEB_INFO("Set default cursor");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::BeginMenu("Audio")) {
                if (ImGui::BeginMenu("Master Volume")) {
                    ImGui::PushItemWidth(100.0f);
                    if (ImGui::SliderFloat("##", &data.options.master_volume, 0.0f, 1.0f, "%.01f")) {
                        app->openal->get_listener().set_gain(data.options.master_volume);

                        DEB_INFO("Changed master volume to {}", data.options.master_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }
                if (ImGui::BeginMenu("Music Volume")) {
                    ImGui::PushItemWidth(100.0f);
                    if (ImGui::SliderFloat("##", &data.options.music_volume, 0.0f, 1.0f, "%.01f")) {
                        music::set_music_gain(data.options.music_volume);

                        DEB_INFO("Changed music volume to {}", data.options.music_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }
                if (ImGui::MenuItem("Enable Music", nullptr, &data.options.enable_music)) {
                    if (data.options.enable_music) {
                        auto& data = app->user_data<Data>();

                        music::play_music_track(data.current_music_track);

                        DEB_INFO("Enabled music");
                    } else {
                        music::stop_music_track();

                        DEB_INFO("Disabled music");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &data.options.save_on_exit)) {
                if (data.options.save_on_exit) {
                    DEB_INFO("The game will be saved on exit");
                } else {
                    DEB_INFO("The game will not be saved on exit");
                }
            }
            if (ImGui::BeginMenu("Skybox")) {
                if (skybox_loader->is_in_use()) {
                    ImGui::RadioButton("None", false);
                    ImGui::RadioButton("Field", false);
                    ImGui::RadioButton("Autumn", false);
                } else {
                    if (ImGui::RadioButton("None", &data.imgui_option.skybox, game_options::NONE)) {
                        if (data.imgui_option.skybox != data.options.skybox) {
                            data.options.skybox = data.imgui_option.skybox;
                            set_skybox(Skybox::None);

                            DEB_INFO("Skybox set to none");
                        }
                    }
                    if (ImGui::RadioButton("Field", &data.imgui_option.skybox, game_options::FIELD)) {
                        if (data.imgui_option.skybox != data.options.skybox) {
                            data.options.skybox = data.imgui_option.skybox;
                            set_skybox(Skybox::Field);

                            DEB_INFO("Skybox set to field");
                        }
                    }
                    if (ImGui::RadioButton("Autumn", &data.imgui_option.skybox, game_options::AUTUMN)) {
                        if (data.imgui_option.skybox != data.options.skybox) {
                            data.options.skybox = data.imgui_option.skybox;
                            set_skybox(Skybox::Autumn);

                            DEB_INFO("Skybox set to autumn");
                        }
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Show Information", nullptr, &show_info)) {
                if (show_info) {
                    DEB_INFO("Show information");
                } else {
                    DEB_INFO("Hide information");
                }
            }
            if (ImGui::BeginMenu("Camera Sensitivity")) {
                ImGui::PushItemWidth(100.0f);
                if (ImGui::SliderFloat("##", &data.options.sensitivity, 0.5f, 2.0f, "%.01f", ImGuiSliderFlags_Logarithmic)) {
                    camera_controller.sensitivity = data.options.sensitivity;

                    DEB_INFO("Changed camera sensitivity to {}", camera_controller.sensitivity);
                }
                ImGui::PopItemWidth();

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::BeginMenu("User Interface")) {
                if (ImGui::MenuItem("Hide Timer", nullptr, &data.options.hide_timer)) {
                    auto& data = app->user_data<Data>();

                    if (data.options.hide_timer) {
                        scene_list.remove(objects.get<gui::Text>("timer_text"_H));

                        DEB_INFO("Hide timer");
                    } else {
                        scene_list.add(objects.get<gui::Text>("timer_text"_H));

                        DEB_INFO("Show timer");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Labeled Board", nullptr, &data.imgui_option.labeled_board)) {
                if (data.imgui_option.labeled_board != data.options.labeled_board) {
                    data.options.labeled_board = data.imgui_option.labeled_board;
                    set_board_paint_texture();

                    if (data.imgui_option.labeled_board) {
                        DEB_INFO("Board paint texture set to labeled");
                    } else {
                        DEB_INFO("Board paint texture set to non-labeled");
                    }
                }
            }

            ImGui::EndMenu();
            HOVERING_GUI()
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", nullptr, false)) {
                window = WindowImGui::ShowAbout;
            }
            if (ImGui::BeginMenu("Game Rules")) {
                if (ImGui::MenuItem("Standard Game", nullptr, false)) {
                    window = WindowImGui::ShowRulesStandardGame;
                }
                if (ImGui::MenuItem("Jump Variant", nullptr, false)) {
                    window = WindowImGui::ShowRulesJumpVariant;
                }
                if (ImGui::MenuItem("Jump Plus Variant", nullptr, false)) {
                    window = WindowImGui::ShowRulesJumpPlusVariant;
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Log Information", nullptr, false)) {
                logging::log_general_information(logging::LogTarget::File);

                DEB_INFO("Logged OpenGL and dependencies information");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", info_file_path.c_str());
            }

            ImGui::EndMenu();
            HOVERING_GUI()
        }

        ImGui::EndMainMenuBar();
    }
}

void SceneGame::imgui_draw_info() {
    ImGui::PushFont(app->user_data<Data>().imgui_info_font);

    const int flags = (
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
    );

    ImGui::Begin("Information", nullptr, flags);
    ImGui::Text("FPS: %.3f", app->get_fps());
    ImGui::Text("OpenGL: %s", gl::get_opengl_version());
    ImGui::Text("Renderer: %s", gl::get_renderer());
    ImGui::End();

    ImGui::PopFont();
}

void SceneGame::imgui_draw_game_over() {
    imgui_draw_window("Game Over", [this]() {
        switch (get_board().ending.type) {
            case BoardEnding::WinnerWhite: {
                const char* message1 = "White player wins!";
                imgui_draw_game_over_message(message1, get_board().ending.reason);
                break;
            }
            case BoardEnding::WinnerBlack: {
                const char* message1 = "Black player wins!";
                imgui_draw_game_over_message(message1, get_board().ending.reason);
                break;
            }
            case BoardEnding::TieBetweenBothPlayers: {
                const char* message1 = "Tie between both players!";
                imgui_draw_game_over_message(message1, get_board().ending.reason);
                break;
            }
            case BoardEnding::None:
                ASSERT(false, "Ending cannot be None");
        }

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        const auto time = timer.get_time_formatted();
        char time_label[64];
        snprintf(time_label, 64, "Time: %s", time.c_str());

        const float window_width = ImGui::GetWindowSize().x;
        const float text_width = ImGui::CalcTextSize(time_label).x;
        ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
        ImGui::Text("%s", time_label);
    },
    [this]() {
        get_board().phase = BoardPhase::None;
    });
}

void SceneGame::imgui_draw_about() {
    imgui_draw_window("About Nine Morris 3D", [this]() {
        ImGui::Text("A 3D implementation of the board game nine men's morris");
        ImGui::Text("Version %u.%u.%u", app->data().version_major, app->data().version_minor, app->data().version_patch);
        ImGui::Separator();
        ImGui::Text("All programming by:");
        ImGui::Text(u8"Simon-Teodor Mărăcine - simonmara.dev@gmail.com");
    });
}

void SceneGame::imgui_draw_could_not_load_game() {
    imgui_draw_window("Error Loading Game", [this]() {
        ImGui::Text("Could not load last game.");
        ImGui::Text("The save game file is either missing or is corrupted.");
        ImGui::Separator();
        ImGui::Text("%s", save_game_file_path.c_str());  // FIXME probably text is too large
    });
}

void SceneGame::imgui_draw_no_last_game() {
    imgui_draw_window("No Last Game", []() {
        ImGui::Text("There is no last saved game.");
    });
}

void SceneGame::imgui_draw_rules_standard_game() {
    imgui_draw_window("Standard Game Rules", []() {
        const char* rules = R"(Each player has nine pieces, either black or white.
A player wins by reducing the opponent to two pieces, or by leaving them without a legal move.
When a player remains with three pieces, they can jump on the board.
A player may take a piece from a mill only if there are no other pieces available.
The game ends with a tie when forty turns take place without any mill.
The game ends with a tie when the exact same move happens for the third time.)";
        ImGui::Text("%s", rules);
    });
}

void SceneGame::imgui_draw_rules_jump_variant() {
    imgui_draw_window("Jump Variant Rules", []() {
        const char* rules = R"(Each player has only three pieces and can jump anywhere on the board.
The first player to form a mill wins.
The game ends with a tie when forty turns take place without any mill.
The game ends with a tie when the exact same move happens for the third time.)";
        ImGui::Text("%s", rules);
    });
}

void SceneGame::imgui_draw_rules_jump_plus_variant() {
    imgui_draw_window("Jump Plus Variant Rules", []() {
        const char* rules = R"(Same rules as the jump variant, but each player has six pieces instead of three.)";
        ImGui::Text("%s", rules);
    });
}

void SceneGame::imgui_draw_debug() {
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.3f", app->get_fps());
    draw_debug_imgui();
    ImGui::Text("Phase: %d", static_cast<int>(get_board().phase));
    ImGui::Text("Turn: %s", get_board().turn == BoardPlayer::White ? "white" : "black");
    ImGui::Text("Turn count: %u", get_board().turn_count);
    ImGui::Text("Must take piece: %s", get_board().must_take_piece ? "true" : "false");
    ImGui::Text("Undo history size: %lu", get_undo_size());
    ImGui::Text("Redo history size: %lu", get_redo_size());
    ImGui::Text("Hovered ID: %.3f", static_cast<float>(app->renderer->get_hovered_id()));
    ImGui::Text("Clicked node: %lu", get_board().clicked_node_index);
    ImGui::Text("Clicked piece: %lu", get_board().clicked_piece_index);
    ImGui::Text("Selected piece: %lu", get_board().selected_piece_index);
    ImGui::Text("Is player's turn: %s", get_board().is_players_turn ? "true" : "false");
    ImGui::Text("Next move: %s", get_board().next_move ? "true" : "false");
    ImGui::Text("Game started: %s", made_first_move ? "true" : "false");
    ImGui::End();

    {
        const float time = app->get_delta() * 1000.0f;
        frames[index] = time;

        if (index < FRAMES_SIZE) {
            index++;
            frames.push_back(time);
        } else {
            frames.push_back(time);
            frames.erase(frames.begin());
        }

        char text[32];
        snprintf(text, 32, "%.3f", time);

        ImGui::Begin("Frame Time");
        ImGui::PlotLines("time (ms)", frames.data(), FRAMES_SIZE, 0, text, 0.0f, 50.0f, ImVec2(200, 60));
        ImGui::End();
    }

    ImGui::Begin("Game");
    const char* state = nullptr;
    switch (game.state) {
        case GameState::NextPlayer:
            state = "NextPlayer";
            break;
        case GameState::HumanBeginMove:
            state = "HumanBeginMove";
            break;
        case GameState::HumanThinkingMove:
            state = "HumanThinkingMove";
            break;
        case GameState::HumanDoingMove:
            state = "HumanDoingMove";
            break;
        case GameState::HumanDoingMoveAndTake:
            state = "HumanDoingMoveAndTake";
            break;
        case GameState::HumanEndMove:
            state = "HumanEndMove";
            break;
        case GameState::ComputerBeginMove:
            state = "ComputerBeginMove";
            break;
        case GameState::ComputerThinkingMove:
            state = "ComputerThinkingMove";
            break;
        case GameState::ComputerDoingMove:
            state = "ComputerDoingMove";
            break;
        case GameState::ComputerDoingMoveAndTake:
            state = "ComputerDoingMoveAndTake";
            break;
        case GameState::ComputerEndMove:
            state = "ComputerEndMove";
            break;
    }
    ImGui::Text("State: %s", state);
    ImGui::Text("White player: %s", game.white_player == GamePlayer::Human ? "Human" : "Computer");
    ImGui::Text("Black player: %s", game.black_player == GamePlayer::Human ? "Human" : "Computer");
    ImGui::End();

    ImGui::Begin("Light Settings");
    if (ImGui::SliderFloat3("Position", glm::value_ptr(app->renderer->directional_light.position), -30.0f, 30.0f)) {
        objects.get<renderables::Quad>("light_bulb"_H)->position = app->renderer->directional_light.position;
    }
    ImGui::SliderFloat3("Ambient color", glm::value_ptr(app->renderer->directional_light.ambient_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse color", glm::value_ptr(app->renderer->directional_light.diffuse_color), 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular color", glm::value_ptr(app->renderer->directional_light.specular_color), 0.0f, 1.0f);
    ImGui::End();

    // If you recompile shaders, uniforms that are set only once need to be reuploaded
    /*
    ImGui::Begin("Shaders");  // TODO see what to do with this
    if (ImGui::Button("board_paint")) {
        // app->data.board_paint_shader->recompile();
    }
    if (ImGui::Button("board")) {
        // app->data.board_wood_shader->recompile();
    }
    if (ImGui::Button("node")) {
        // app->data.node_shader->recompile();
    }
    if (ImGui::Button("origin")) {
        // app->renderer->get_origin_shader()->recompile();
    }
    if (ImGui::Button("outline")) {
        // app->renderer->get_outline_shader()->recompile();
    }
    if (ImGui::Button("piece")) {
        // app->data.piece_shader->recompile();
    }
    if (ImGui::Button("quad2d")) {
        // app->gui_renderer->get_quad2d_shader()->recompile();
    }
    if (ImGui::Button("quad3d")) {
        // app->renderer->get_quad3d_shader()->recompile();
    }
    if (ImGui::Button("screen_quad")) {
        // app->renderer->get_screen_quad_shader()->recompile();
    }
    if (ImGui::Button("shadow")) {
        // app->renderer->get_shadow_shader()->recompile();
    }
    if (ImGui::Button("skybox")) {
        // app->renderer->get_skybox_shader()->recompile();
    }
    if (ImGui::Button("text")) {
        // app->gui_renderer->get_text_shader()->recompile();
    }
    ImGui::End();
    */

    const glm::vec3& position = camera_controller.get_position();
    const glm::vec3& rotation = camera_controller.get_rotation();

    ImGui::Begin("Camera");
    ImGui::Text("Position: %.3f, %.3f, %.3f", position.x, position.y, position.z);
    ImGui::Text("Pitch: %.3f", rotation.x);
    ImGui::Text("Yaw: %.3f", rotation.y);
    ImGui::Text("Angle around point: %.3f", camera_controller.get_angle_around_point());
    ImGui::Text("Distance to point: %.3f", camera_controller.get_distance_to_point());
    ImGui::End();

    ImGui::Begin("Light Space Matrix");
    ImGui::SliderFloat("Left", &app->renderer->light_space.left, -10.0f, 10.0f);
    ImGui::SliderFloat("Right", &app->renderer->light_space.right, -10.0f, 10.0f);
    ImGui::SliderFloat("Bottom", &app->renderer->light_space.bottom, -10.0f, 10.0f);
    ImGui::SliderFloat("Top", &app->renderer->light_space.top, -10.0f, 10.0f);
    ImGui::SliderFloat("Near", &app->renderer->light_space.lens_near, 0.1f, 2.0f);
    ImGui::SliderFloat("Far", &app->renderer->light_space.lens_far, 2.0f, 50.0f);
    ImGui::SliderFloat("Position divisor", &app->renderer->light_space.position_divisor, 1.0f, 10.0f);
    ImGui::End();
}

void SceneGame::imgui_draw_game_over_message(std::string_view message1, std::string_view message2) {
    ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

    const float window_width = ImGui::GetWindowSize().x;
    float text_width;

    text_width = ImGui::CalcTextSize(message1.data()).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text("%s", message1.data()); ImGui::SameLine();
    ImGui::Dummy(ImVec2(20.0f, 0.0f));

    text_width = ImGui::CalcTextSize(message2.data()).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text("%s", message2.data()); ImGui::SameLine();
    ImGui::Dummy(ImVec2(20.0f, 0.0f));

    ImGui::Dummy(ImVec2(0.0f, 2.0f));
}

void SceneGame::imgui_draw_window(const char* title, const std::function<void()>& contents,
        const std::function<void()>& ok_callback) {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup(title);

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title, nullptr, window_flags)) {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        contents();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            window = WindowImGui::None;
            ok_callback();
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::EndPopup();
        HOVERING_GUI()
    }
    ImGui::PopFont();
}

void SceneGame::imgui_initialize_options() {  // FIXME this is no longer needed
    auto& data = app->user_data<Data>();

    data.imgui_option.skybox = data.options.skybox;
    data.imgui_option.labeled_board = data.options.labeled_board;
    data.imgui_option.scene = data.options.scene;
}
