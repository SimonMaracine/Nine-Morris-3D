#pragma once

#include <engine/engine_application.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/minimax/minimax_thread.h"
#include "game/assets_load.h"
#include "game/point_camera_controller.h"
#include "game/undo_redo_state.h"
#include "game/keyboard_controls.h"
#include "game/game_context.h"
#include "game/timer.h"

template<typename S, typename B>
class SceneGame {
public:
    SceneGame() {
        App = static_cast<S*>(this)->App;
        scene = static_cast<S*>(this);
    }

    virtual ~SceneGame() = default;

    SceneGame(const SceneGame&) = delete;
    SceneGame& operator=(const SceneGame&) = delete;
    SceneGame(SceneGame&&) = delete;
    SceneGame& operator=(SceneGame&&) = delete;

    virtual void setup_and_add_model_pieces() = 0;
    virtual void setup_entities() = 0;
    virtual void initialize_pieces() = 0;
    virtual void imgui_draw_debug() = 0;

    void setup_and_add_model_board();
    void setup_and_add_model_board_paint();
    void setup_and_add_model_piece(Index index, const glm::vec3& position);
    void setup_and_add_model_nodes();
    void setup_and_add_model_node(Index index, const glm::vec3& position);
    void setup_piece_on_node(Index index, Index node_index);
    void setup_camera();
    void setup_and_add_turn_indicator();
    void setup_and_add_timer_text();
    void setup_wait_indicator();
    void setup_computer_thinking_indicator();
    void setup_light_bulb();

    void initialize_piece(Index index, std::shared_ptr<gl::Texture> diffuse_texture);
    void initialize_piece_no_normal(Index index, std::shared_ptr<gl::Texture> diffuse_texture);

    void release_piece_material_instances();
    void change_skybox();
    void change_board_paint_texture();

    void update_listener();
    void update_cursor();
    void update_turn_indicator();
    void update_wait_indicator();
    void update_computer_thinking_indicator();
    void update_timer_text();
    void update_after_human_move(bool did_action, bool switched_turn, bool must_take_or_took_piece);
    void update_after_computer_move(bool switched_turn);
    void update_game_state();
    void update_all_imgui();

    void set_skybox(Skybox skybox);
    void set_board_paint_texture();

    void save_game();
    void load_game();
    void undo();
    void redo();

    std::unique_ptr<assets_load::SkyboxLoader> skybox_loader;
    std::unique_ptr<assets_load::BoardPaintTextureLoader> board_paint_texture_loader;

    // Game-related
    Camera camera;
    PointCameraController camera_controller {&camera};
    KeyboardControls keyboard;
    GameContext game;
    MinimaxThread minimax_thread;
    Timer timer;

    bool made_first_move = false;
    bool show_keyboard_controls = false;

    glm::vec3 default_camera_position = glm::vec3(0.0f);
    std::string save_game_file_name;

    // GUI-related
    bool show_wait_indicator = false;
    bool show_computer_thinking_indicator = false;
private:
    Application* App = nullptr;  // Avoid conflict with derived classes
    S* scene = nullptr;
};

template<typename S, typename B>
void SceneGame<S, B>::setup_and_add_model_board() {
    auto& board = scene->board;

    board.model->scale = WORLD_SCALE;
    board.model->vertex_array = App->res.vertex_array["board_wood"_H];
    board.model->index_buffer = App->res.index_buffer["board_wood"_H];
    board.model->material = App->res.material_instance["board_wood"_H];
    board.model->cast_shadow = true;
    board.model->bounding_box = std::make_optional<object::Model::BoundingBox>();
    board.model->bounding_box->id = identifier::null;
    board.model->bounding_box->size = BOARD_BOUNDING_BOX;
    board.model->bounding_box->sort = false;

    scene->scene_list.add(board.model);
    // App->renderer->add_model(board.model);

    DEB_DEBUG("Setup model board");
}

template<typename S, typename B>
void SceneGame<S, B>::setup_and_add_model_board_paint() {
    const auto& board = scene->board;

    board.paint_model->position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model->scale = WORLD_SCALE;
    board.paint_model->vertex_array = App->res.vertex_array["board_paint"_H];
    board.paint_model->index_buffer = App->res.index_buffer["board_paint"_H];
    board.paint_model->material = App->res.material_instance["board_paint"_H];

    scene->scene_list.add(board.paint_model);
    // App->renderer->add_model(board.paint_model);

    DEB_DEBUG("Setup model board paint");
}

template<typename S, typename B>
void SceneGame<S, B>::setup_and_add_model_piece(Index index, const glm::vec3& position) {
    auto& data = App->user_data<Data>();

    const Piece& piece = scene->board.pieces.at(index);

    const auto id = piece.type == PieceType::White ? "white_piece"_H : "black_piece"_H;

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->scale = WORLD_SCALE;
    piece.model->vertex_array = App->res.vertex_array[id];
    piece.model->index_buffer = App->res.index_buffer[id];
    piece.model->material = App->res.material_instance[hs("piece" + std::to_string(index))];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->cast_shadow = true;
    piece.model->bounding_box = std::make_optional<object::Model::BoundingBox>();
    piece.model->bounding_box->id = data.piece_ids[index];
    piece.model->bounding_box->size = PIECE_BOUNDING_BOX;

    scene->scene_list.add(piece.model);
    // App->renderer->add_model(piece.model);

    piece.source->set_position(position);
    piece.source->set_reference_distance(6.0f);

    DEB_DEBUG("Setup model piece {}", index);
}

template<typename S, typename B>
void SceneGame<S, B>::setup_and_add_model_nodes() {
    for (size_t i = 0; i < MAX_NODES; i++) {
        setup_and_add_model_node(i, NODE_POSITIONS[i]);
    }
}

template<typename S, typename B>
void SceneGame<S, B>::setup_and_add_model_node(Index index, const glm::vec3& position) {
    auto& data = App->user_data<Data>();

    const Node& node = scene->board.nodes.at(index);

    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->vertex_array = App->res.vertex_array["node"_H];
    node.model->index_buffer = App->res.index_buffer["node"_H];
    node.model->material = App->res.material_instance[hs("node" + std::to_string(index))];
    node.model->bounding_box = std::make_optional<object::Model::BoundingBox>();
    node.model->bounding_box->id = data.node_ids[index];
    node.model->bounding_box->size = NODE_BOUNDING_BOX;

    scene->scene_list.add(node.model);
    // App->renderer->add_model(node.model);

    DEB_DEBUG("Setup model node {}", index);
}

template<typename S, typename B>
void SceneGame<S, B>::setup_piece_on_node(Index index, Index node_index) {
    setup_and_add_model_piece(index, PIECE_INDEX_POSITION(node_index));

    scene->board.pieces.at(index).node_index = node_index;
    scene->board.nodes.at(node_index).piece_index = index;
}

template<typename S, typename B>
void SceneGame<S, B>::setup_camera() {
    auto& data = App->user_data<Data>();

    static constexpr float PITCH = 47.0f;
    static constexpr float DISTANCE_TO_POINT = 8.0f;

    scene->camera = Camera {};

    scene->camera_controller = PointCameraController {
        &scene->camera,
        App->data().width,
        App->data().height,
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT,
        PITCH,
        data.options.sensitivity
    };

    scene->default_camera_position = scene->camera_controller.get_position();

    scene->camera_controller = PointCameraController {
        &scene->camera,
        App->data().width,
        App->data().height,
        LENS_FOV,
        LENS_NEAR,
        LENS_FAR,
        glm::vec3(0.0f),
        DISTANCE_TO_POINT + 0.7f,
        PITCH,
        data.options.sensitivity
    };

    App->renderer->set_camera_controller(&scene->camera_controller);
    update_listener();

    DEB_DEBUG("Setup camera");
}

template<typename S, typename B>
void SceneGame<S, B>::setup_and_add_turn_indicator() {
    auto turn_indicator = scene->objects.template add<gui::Image>(
        "turn_indicator"_H, App->res.texture["white_indicator"_H]
    );
    // auto turn_indicator = scene->scene.image.load(
    //     "turn_indicator"_H, App->res.texture["white_indicator"_H]
    // );

    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right);
    turn_indicator->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);

    scene->scene_list.add(turn_indicator);
    // App->gui_renderer->add_widget(turn_indicator.get());
}

template<typename S, typename B>
void SceneGame<S, B>::setup_and_add_timer_text() {
    auto& data = App->user_data<Data>();

    auto timer_text = scene->objects.template add<gui::Text>(
        "timer_text"_H,
        App->res.font["open_sans"_H],
        "00:00",
        1.5f,
        glm::vec3(0.9f)
    );
    // auto timer_text = scene->scene.text.load(
    //     "timer_text"_H,
    //     App->res.font["open_sans"_H],
    //     "00:00",
    //     1.5f,
    //     glm::vec3(0.9f)
    // );

    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        scene->scene_list.add(timer_text);
        // App->gui_renderer->add_widget(timer_text.get());
    }
}

template<typename S, typename B>
void SceneGame<S, B>::setup_wait_indicator() {
    auto wait_indicator = scene->objects.template add<gui::Image>(
        "wait_indicator"_H, App->res.texture["wait_indicator"_H]
    );
    // auto wait_indicator = scene->scene.image.load(
    //     "wait_indicator"_H, App->res.texture["wait_indicator"_H]
    // );

    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right);
    wait_indicator->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

template<typename S, typename B>
void SceneGame<S, B>::setup_computer_thinking_indicator() {
    auto computer_thinking_indicator = scene->objects.template add<gui::Image>(
        "computer_thinking_indicator"_H, App->res.texture["computer_thinking_indicator"_H]
    );
    // auto computer_thinking_indicator = scene->scene.image.load(
    //     "computer_thinking_indicator"_H, App->res.texture["computer_thinking_indicator"_H]
    // );

    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right);
    computer_thinking_indicator->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, WIDGET_LOWEST_RESOLUTION, WIDGET_HIGHEST_RESOLUTION);
}

template<typename S, typename B>
void SceneGame<S, B>::setup_light_bulb() {
    auto light_bulb = scene->objects.template add<object::Quad>("light_bulb"_H);
    // auto light_bulb = scene->scene.quad.load("light_bulb"_H);

    light_bulb->texture = App->res.texture["light_bulb"_H];
}

template<typename S, typename B>
void SceneGame<S, B>::initialize_piece(Index index, std::shared_ptr<gl::Texture> diffuse_texture) {
    auto material_instance = App->res.material_instance.load(
        hs("piece" + std::to_string(index)),
        App->res.material["tinted_wood"_H]
    );

    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_texture("u_material.normal"_H, App->res.texture["piece_normal"_H], 1);
    material_instance->set_vec3("u_material.tint"_H, DEFAULT_TINT);
}

template<typename S, typename B>
void SceneGame<S, B>::initialize_piece_no_normal(Index index, std::shared_ptr<gl::Texture> diffuse_texture) {
    auto material_instance = App->res.material_instance.load(
        hs("piece" + std::to_string(index)),
        App->res.material["tinted_wood"_H]
    );

    material_instance->set_texture("u_material.diffuse"_H, diffuse_texture, 0);
    material_instance->set_vec3("u_material.specular"_H, glm::vec3(0.2f));
    material_instance->set_float("u_material.shininess"_H, 4.0f);
    material_instance->set_vec3("u_material.tint"_H, DEFAULT_TINT);
}

template<typename S, typename B>
void SceneGame<S, B>::release_piece_material_instances() {
    for (size_t i = 0; i < MAX_PIECES; i++) {
        App->res.material_instance.release(hs("piece" + std::to_string(i)));
    }
}

template<typename S, typename B>
void SceneGame<S, B>::change_skybox() {
    auto& data = App->user_data<Data>();

    const std::array<std::shared_ptr<TextureData>, 6> texture_data = {
        App->res.texture_data["skybox_px"_H],
        App->res.texture_data["skybox_nx"_H],
        App->res.texture_data["skybox_py"_H],
        App->res.texture_data["skybox_ny"_H],
        App->res.texture_data["skybox_pz"_H],
        App->res.texture_data["skybox_nz"_H]
    };

    auto texture = App->res.texture_3d.force_load("skybox"_H, texture_data);
    App->renderer->set_skybox(texture);

    if (data.options.skybox == game_options::FIELD) {  // FIXME this is not dry
        App->renderer->light = LIGHT_FIELD;
        App->renderer->light_space = SHADOWS_FIELD;
    } else if (data.options.skybox == game_options::AUTUMN) {
        App->renderer->light = LIGHT_AUTUMN;
        App->renderer->light_space = SHADOWS_AUTUMN;
    } else if (data.options.skybox == game_options::NONE) {
        App->renderer->light = LIGHT_NONE;
        App->renderer->light_space = SHADOWS_NONE;
    } else {
        ASSERT(false, "Invalid skybox");
    }

    App->res.texture_data.release("skybox"_H);
}

template<typename S, typename B>
void SceneGame<S, B>::change_board_paint_texture() {
    auto& data = App->user_data<Data>();

    gl::TextureSpecification specification;
    specification.mag_filter = gl::Filter::Linear;
    specification.mipmap_levels = 4;
    specification.bias = -1.0f;
    specification.anisotropic_filtering = data.launcher_options.anisotropic_filtering;

    auto diffuse_texture = App->res.texture.force_load(
        "board_paint_diffuse"_H,
        App->res.texture_data["board_paint_diffuse"_H],
        specification
    );

    App->res.material_instance["board_paint"_H]->set_texture("u_material.diffuse"_H, diffuse_texture, 0);

    App->res.texture_data.release("board_paint_diffuse"_H);
}

template<typename S, typename B>
void SceneGame<S, B>::update_listener() {
    auto& listener = App->openal->get_listener();

    listener.set_position(scene->camera_controller.get_position());

    listener.set_look_at_and_up(
        scene->camera_controller.get_point() - scene->camera_controller.get_position(),
        glm::rotate(UP_VECTOR, scene->camera_controller.get_rotation().y, UP_VECTOR)
    );
}

template<typename S, typename B>
void SceneGame<S, B>::update_cursor() {
    auto& data = App->user_data<Data>();

    if (data.options.custom_cursor) {
        if (scene->board.must_take_piece) {
            App->window->set_cursor(data.cross_cursor);

            scene->objects.template get<object::Quad>("keyboard_controls"_H)->texture = App->res.texture["keyboard_controls_cross"_H];
            // scene->scene.quad["keyboard_controls"_H]->texture = App->res.texture["keyboard_controls_cross"_H];
        } else {
            App->window->set_cursor(data.arrow_cursor);

            scene->objects.template get<object::Quad>("keyboard_controls"_H)->texture = App->res.texture["keyboard_controls_default"_H];
            // scene->scene.quad["keyboard_controls"_H]->texture = App->res.texture["keyboard_controls_default"_H];
        }
    }
}

template<typename S, typename B>
void SceneGame<S, B>::update_turn_indicator() {
    if (scene->board.turn == BoardPlayer::White) {
        scene->objects.template get<gui::Image>("turn_indicator"_H)->set_image(App->res.texture["white_indicator"_H]);
        // scene->scene.image["turn_indicator"_H]->set_image(App->res.texture["white_indicator"_H]);
    } else {
        scene->objects.template get<gui::Image>("turn_indicator"_H)->set_image(App->res.texture["black_indicator"_H]);
        // scene->scene.image["turn_indicator"_H]->set_image(App->res.texture["black_indicator"_H]);
    }
}

template<typename S, typename B>
void SceneGame<S, B>::update_wait_indicator() {
    if (!scene->board.next_move) {
        if (!scene->show_wait_indicator) {
            scene->scene_list.add(scene->objects.template get<gui::Image>("wait_indicator"_H));
            // App->gui_renderer->add_widget(scene->scene.image["wait_indicator"_H].get());
            scene->show_wait_indicator = true;
        }
    } else {
        if (scene->show_wait_indicator) {
            scene->scene_list.remove(scene->objects.template get<gui::Image>("wait_indicator"_H));
            // App->gui_renderer->remove_widget(scene->scene.image["wait_indicator"_H].get());
            scene->show_wait_indicator = false;
        }
    }
}

template<typename S, typename B>
void SceneGame<S, B>::update_computer_thinking_indicator() {
    if (scene->game.state == GameState::ComputerThinkingMove) {
        if (!scene->show_computer_thinking_indicator) {
            scene->scene_list.add(scene->objects.template get<gui::Image>("computer_thinking_indicator"_H));
            // App->gui_renderer->add_widget(scene->scene.image["computer_thinking_indicator"_H].get());
            scene->show_computer_thinking_indicator = true;
        }
    } else {
        if (scene->show_computer_thinking_indicator) {
            scene->scene_list.remove(scene->objects.template get<gui::Image>("computer_thinking_indicator"_H));
            // App->gui_renderer->remove_widget(scene->scene.image["computer_thinking_indicator"_H].get());
            scene->show_computer_thinking_indicator = false;
        }
    }
}

template<typename S, typename B>
void SceneGame<S, B>::update_timer_text() {
    char time[32];
    scene->timer.get_time_formatted(time);
    scene->objects.template get<gui::Text>("timer_text"_H)->set_text(time);
    // scene->scene.text["timer_text"_H]->set_text(time);
}

template<typename S, typename B>
void SceneGame<S, B>::update_after_human_move(bool did_action, bool switched_turn, bool must_take_or_took_piece) {
    if (did_action) {
        scene->game.state = GameState::HumanDoingMove;
    }

    if (did_action && !scene->made_first_move && !scene->timer.is_running()) {
        scene->timer.start();
        scene->made_first_move = true;
    }

    if (scene->board.phase == BoardPhase::GameOver) {
        scene->timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator();
    }

    if (must_take_or_took_piece) {
        update_cursor();
    }

    scene->imgui_layer.can_undo = scene->undo_redo_state.undo.size() > 0;
    scene->imgui_layer.can_redo = scene->undo_redo_state.redo.size() > 0;
}

template<typename S, typename B>
void SceneGame<S, B>::update_after_computer_move(bool switched_turn) {
    scene->game.state = GameState::ComputerDoingMove;

    if (!scene->made_first_move && !scene->timer.is_running()) {
        scene->timer.start();
        scene->made_first_move = true;
    }

    if (scene->board.phase == BoardPhase::GameOver) {
        scene->timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator();
    }

    scene->imgui_layer.can_undo = scene->undo_redo_state.undo.size() > 0;
    scene->imgui_layer.can_redo = scene->undo_redo_state.redo.size() > 0;
}

template<typename S, typename B>
void SceneGame<S, B>::update_game_state() {
    switch (scene->game.state) {
        case GameState::MaybeNextPlayer:
            switch (scene->board.turn) {
                case BoardPlayer::White:
                    switch (scene->game.white_player) {
                        case GamePlayer::None:
                            ASSERT(false, "Player must not be None");
                            break;
                        case GamePlayer::Human:
                            scene->game.state = GameState::HumanBeginMove;
                            break;
                        case GamePlayer::Computer:
                            scene->game.state = GameState::ComputerBeginMove;
                            break;
                    }
                    break;
                case BoardPlayer::Black:
                    switch (scene->game.black_player) {
                        case GamePlayer::None:
                            ASSERT(false, "Player must not be None");
                            break;
                        case GamePlayer::Human:
                            scene->game.state = GameState::HumanBeginMove;
                            break;
                        case GamePlayer::Computer:
                            scene->game.state = GameState::ComputerBeginMove;
                            break;
                    }
                    break;
            }
            break;
        case GameState::HumanBeginMove:
            scene->game.begin_human_move();
            scene->game.state = GameState::HumanThinkingMove;
            break;
        case GameState::HumanThinkingMove:
            break;
        case GameState::HumanDoingMove:
            if (scene->board.next_move) {
                scene->game.state = GameState::HumanEndMove;
            }
            break;
        case GameState::HumanEndMove:
            scene->game.end_human_move();
            scene->game.state = GameState::MaybeNextPlayer;
            break;
        case GameState::ComputerBeginMove:
            scene->game.begin_computer_move();
            scene->game.state = GameState::ComputerThinkingMove;
            break;
        case GameState::ComputerThinkingMove:
            if (!scene->minimax_thread.is_running()) {
                scene->minimax_thread.join();

                const bool switched_turn = scene->game.end_computer_move();

                update_after_computer_move(switched_turn);
            }
            break;
        case GameState::ComputerDoingMove:
            if (scene->board.next_move) {
                scene->game.state = GameState::ComputerEndMove;
            }
            break;
        case GameState::ComputerEndMove:
            scene->game.state = GameState::MaybeNextPlayer;
            break;
    }
}

template<typename S, typename B>
void SceneGame<S, B>::update_all_imgui() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(App->data().width, App->data().height);
    io.DeltaTime = App->get_delta();

    auto& imgui_layer = scene->imgui_layer;

    if (scene->board.phase == BoardPhase::GameOver && scene->board.next_move) {
        imgui_layer.window = WindowImGui::ShowGameOver;
    }

    imgui_layer.draw_menu_bar();

    switch (imgui_layer.window) {
        case WindowImGui::None:
            // Do nothing
            break;
        case WindowImGui::ShowAbout:
            imgui_layer.draw_about();
            break;
        case WindowImGui::ShowCouldNotLoadGame:
            imgui_layer.draw_could_not_load_game();
            break;
        case WindowImGui::ShowNoLastGame:
            imgui_layer.draw_no_last_game();
            break;
        case WindowImGui::ShowGameOver:
            imgui_layer.draw_game_over();
            break;
        case WindowImGui::ShowRulesStandardGame:
            imgui_layer.draw_rules_standard_game();
            break;
        case WindowImGui::ShowRulesJumpVariant:
            imgui_layer.draw_rules_jump_variant();
            break;
        case WindowImGui::ShowRulesJumpPlusVariant:
            imgui_layer.draw_rules_jump_plus_variant();
            break;
    }

    if (imgui_layer.show_info) {
        imgui_layer.draw_info();
    }

#ifdef NM3D_PLATFORM_DEBUG
    imgui_layer.draw_debug();
#endif
}

template<typename S, typename B>
void SceneGame<S, B>::set_skybox(Skybox skybox) {
    if (skybox == Skybox::None) {
        App->renderer->set_skybox(nullptr);
        return;
    }

    auto& data = App->user_data<Data>();

    scene->skybox_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.skybox);
}

template<typename S, typename B>
void SceneGame<S, B>::set_board_paint_texture() {
    auto& data = App->user_data<Data>();

    scene->board_paint_texture_loader->start_loading_thread(
        data.launcher_options.texture_quality,
        data.options.labeled_board
    );
}

template<typename S, typename B>
void SceneGame<S, B>::save_game() {
    scene->board.finalize_pieces_state();

    B board_serialized;
    scene->board.to_serialized(board_serialized);

    save_load::SavedGame<B> saved_game;
    saved_game.board_serialized = board_serialized;
    saved_game.camera_controller = scene->camera_controller;
    saved_game.time = scene->timer.get_time();

    time_t current;
    time(&current);
    saved_game.date = ctime(&current);

    saved_game.undo_redo_state = scene->undo_redo_state;
    saved_game.white_player = scene->game.white_player;
    saved_game.black_player = scene->game.black_player;

    try {
        save_load::save_game_to_file(saved_game, scene->save_game_file_name);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not save game: {}", e.what());

        save_load::handle_save_file_not_open_error();
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not save game: {}", e.what());
    }
}

template<typename S, typename B>
void SceneGame<S, B>::load_game() {
    scene->board.finalize_pieces_state();

    save_load::SavedGame<B> saved_game;

    try {
        save_load::load_game_from_file(saved_game, scene->save_game_file_name);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not load game: {}", e.what());

        save_load::handle_save_file_not_open_error();

        scene->imgui_layer.window = WindowImGui::ShowCouldNotLoadGame;
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not load game: {}", e.what());  // TODO maybe delete file

        scene->imgui_layer.window = WindowImGui::ShowCouldNotLoadGame;
        return;
    }

    scene->board.from_serialized(saved_game.board_serialized);
    scene->camera_controller = saved_game.camera_controller;
    scene->timer = Timer {App, saved_game.time};
    scene->undo_redo_state = std::move(saved_game.undo_redo_state);
    scene->game.white_player = saved_game.white_player;
    scene->game.black_player = saved_game.black_player;

    // Set camera pointer lost in serialization
    scene->camera_controller.set_camera(&scene->camera);

    scene->made_first_move = false;

    update_cursor();
    update_turn_indicator();
}

template<typename S, typename B>
void SceneGame<S, B>::undo() {
    ASSERT(!scene->undo_redo_state.undo.empty(), "Undo history must not be empty");

    if (!scene->board.next_move) {
        DEB_WARNING("Cannot undo when pieces are in air");
        return;
    }

    const bool undo_game_over = scene->board.phase == BoardPhase::None;

    using State = typename UndoRedoState<B>::State;

    B board_serialized;
    scene->board.to_serialized(board_serialized);

    State current_state = { board_serialized, scene->camera_controller };
    const State& previous_state = scene->undo_redo_state.undo.back();

    scene->board.from_serialized(previous_state.board_serialized);
    scene->camera_controller = previous_state.camera_controller;

    scene->undo_redo_state.undo.pop_back();
    scene->undo_redo_state.redo.push_back(current_state);

    DEB_DEBUG("Undid move; popped from undo stack and pushed onto redo stack");

    scene->game.state = GameState::MaybeNextPlayer;
    scene->made_first_move = scene->board.turn_count != 0;

    if (undo_game_over) {
        scene->timer.start();
    }

    update_cursor();
    update_turn_indicator();
}

template<typename S, typename B>
void SceneGame<S, B>::redo() {
    ASSERT(!scene->undo_redo_state.redo.empty(), "Redo history must not be empty");

    if (!scene->board.next_move) {
        DEB_WARNING("Cannot redo when pieces are in air");
        return;
    }

    using State = typename UndoRedoState<B>::State;

    B board_serialized;
    scene->board.to_serialized(board_serialized);

    State current_state = { board_serialized, scene->camera_controller };
    const State& previous_state = scene->undo_redo_state.redo.back();

    scene->board.from_serialized(previous_state.board_serialized);
    scene->camera_controller = previous_state.camera_controller;

    scene->undo_redo_state.redo.pop_back();
    scene->undo_redo_state.undo.push_back(current_state);

    DEB_DEBUG("Redid move; popped from redo stack and pushed onto undo stack");

    scene->game.state = GameState::MaybeNextPlayer;
    scene->made_first_move = scene->board.turn_count != 0;

    const bool redo_game_over = scene->board.phase == BoardPhase::None;

    if (redo_game_over) {
        scene->timer.stop();
        scene->board.phase = BoardPhase::GameOver;  // Make the game over screen show up again
    }

    update_cursor();
    update_turn_indicator();
}
