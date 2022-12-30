#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/boards/jump_board.h"
#include "game/entities/serialization/jump_board_serialized.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/scenes/jump_variant_scene.h"
#include "game/scenes/imgui_layer.h"
#include "game/scenes/common.h"
#include "game/game_options.h"
#include "game/save_load.h"
#include "game/assets.h"
#include "game/assets_load.h"
#include "game/point_camera_controller.h"
#include "other/constants.h"
#include "other/data.h"
#include "other/options.h"
#include "other/options_gracefully.h"

using namespace encrypt;

void JumpVariantScene::on_start() {
    auto& data = app->user_data<Data>();

    setup_entities();

    setup_and_add_model_board();
    setup_and_add_model_board_paint();
    setup_and_add_model_nodes();  // Nodes first
    setup_and_add_model_pieces();

    setup_camera(app, *this);
    setup_widgets();
    update_turn_indicator();

    keyboard = KeyboardControls {app, &board, app->res.quad["keyboard_controls"_h]};
    keyboard.post_initialize();

    undo_redo_state = UndoRedoState<JumpBoardSerialized> {};

    minimax_thread = MinimaxThread {&board};

    game = GameContext {
        static_cast<GamePlayer>(data.options.white_player),
        static_cast<GamePlayer>(data.options.black_player),
        &board,
        &minimax_thread
    };

    timer = Timer {app};

    board.app = app;
    board.keyboard = &keyboard;
    board.camera_controller = &camera_controller;
    board.undo_redo_state = &undo_redo_state;

    app->window->set_cursor(data.options.custom_cursor ? data.arrow_cursor : 0);

#ifdef NM3D_PLATFORM_DEBUG
    app->renderer->origin = true;
    app->renderer->add_quad(app->res.quad["light_bulb"_h]);
#endif

    imgui_layer.update();

    camera_controller.go_towards_position(default_camera_position);
    camera_controller.setup_events(app);

    // Can dispose of these
    app->res.texture_data.clear();
    app->res.sound_data.clear();
}

void JumpVariantScene::on_stop() {
    auto& data = app->user_data<Data>();

    options_gracefully::save_to_file<game_options::GameOptions>(
        game_options::GAME_OPTIONS_FILE, data.options
    );

    if (data.options.save_on_exit && !app->running && made_first_move) {
        save_game();
    }

#ifdef NM3D_PLATFORM_DEBUG
    app->renderer->origin = false;
#endif

    imgui_layer.reset();
    camera_controller.remove_events(app);
    app->renderer->clear();
    app->gui_renderer->clear();

    made_first_move = false;

    if (skybox_loader != nullptr) {
        if (skybox_loader->joinable()) {
            skybox_loader->join();
        }
    }
}

void JumpVariantScene::on_awake() {
    auto& data = app->user_data<Data>();

    imgui_layer = ImGuiLayer<JumpVariantScene, JumpBoardSerialized> {app, this};

    if (data.launcher_options.normal_mapping) {
        initialize_board(app);
        initialize_board_paint(app);
        initialize_pieces(app);
    } else {
        initialize_board_no_normal(app);
        initialize_board_paint_no_normal(app);
        initialize_pieces_no_normal(app);
    }
    initialize_nodes(app);

    initialize_keyboard_controls(app);
#ifdef NM3D_PLATFORM_DEBUG
    initialize_light_bulb(app);
#endif

    initialize_skybox(app);
    initialize_light(app);
    initialize_indicators_textures(app);

    app->evt.add_event<MouseButtonPressedEvent, &JumpVariantScene::on_mouse_button_pressed>(this);
    app->evt.add_event<MouseButtonReleasedEvent, &JumpVariantScene::on_mouse_button_released>(this);
    app->evt.add_event<KeyPressedEvent, &JumpVariantScene::on_key_pressed>(this);
    app->evt.add_event<KeyReleasedEvent, &JumpVariantScene::on_key_released>(this);
    app->evt.add_event<WindowResizedEvent, &JumpVariantScene::on_window_resized>(this);

    auto track = app->res.music_track.load("music"_h, app->res.sound_data["music"_h]);
    current_music_track = track;

    if (data.options.enable_music) {
        music::play_music_track(track);
    }

    using namespace assets_load;

    skybox_loader = std::make_unique<SkyboxLoader>(
        skybox, std::bind(change_skybox, app)
    );
    board_paint_texture_loader = std::make_unique<BoardPaintTextureLoader>(
        board_paint_texture, std::bind(change_board_paint_texture, app)
    );
}

void JumpVariantScene::on_update() {
    if (!imgui_layer.hovering_gui) {
        camera_controller.update(app->get_delta());
        board.update_nodes(app->renderer->get_hovered_id());
        board.update_pieces(app->renderer->get_hovered_id());

        // Update listener position, look at and up vectors every frame
        app->openal->get_listener().set_position(camera_controller.get_position());
        app->openal->get_listener().set_look_at_and_up(
            camera_controller.get_point() - camera_controller.get_position(),
            glm::rotate(UP_VECTOR, camera_controller.get_rotation().y, UP_VECTOR)
        );
    }

    board.move_pieces();
    timer.update();

    update_game_state();

    // update_timer_text();
    update_wait_indicator();
    update_computer_thinking_indicator();

    skybox_loader->update(app);
    board_paint_texture_loader->update(app);
}

void JumpVariantScene::on_fixed_update() {
    camera_controller.update_friction();
}

void JumpVariantScene::on_imgui_update() {
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
    } else if (board.phase == BoardPhase::GameOver && board.next_move) {
        imgui_layer.draw_game_over();
    }

    if (imgui_layer.show_info && !imgui_layer.show_about) {
        imgui_layer.draw_info();
    }

#ifdef NM3D_PLATFORM_DEBUG
    imgui_layer.draw_debug();
#endif
}

void JumpVariantScene::on_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void JumpVariantScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::LEFT) {
        const bool valid_phases = (
            board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces
        );

        if (board.next_move && board.is_players_turn && valid_phases) {
            const auto flags = board.release(app->renderer->get_hovered_id());

            update_after_human_move(
                flags.did_action, flags.switched_turn, flags.must_take_piece_or_took_piece
            );
        }

        if (show_keyboard_controls) {
            app->renderer->remove_quad(app->res.quad["keyboard_controls"_h]);
            show_keyboard_controls = false;
        }
    }
}

void JumpVariantScene::on_key_pressed(const KeyPressedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    switch (event.key) {
        case input::Key::UP:
        case input::Key::DOWN:
        case input::Key::LEFT:
        case input::Key::RIGHT:
        case input::Key::ENTER:
            if (!show_keyboard_controls) {
                app->renderer->add_quad(app->res.quad["keyboard_controls"_h]);
                show_keyboard_controls = true;
                return;
            }
        default:
            break;
    }

    using KB = KeyboardControls;

    switch (event.key) {
        case input::Key::UP:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Up, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::DOWN:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Down, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::LEFT:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Left, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::RIGHT:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Right, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::ENTER: {
            const bool valid_phases = (
                board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces
            );

            if (board.next_move && board.is_players_turn && valid_phases) {
                const auto flags = keyboard.click_and_release();

                update_after_human_move(
                    flags.did_action, flags.switched_turn, flags.must_take_piece_or_took_piece
                );
            }
            break;
        }
        default:
            break;
    }
}

void JumpVariantScene::on_key_released(const KeyReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.key == input::Key::SPACE) {
        camera_controller.go_towards_position(default_camera_position);
    }
}

void JumpVariantScene::on_window_resized(const WindowResizedEvent& event) {
    camera.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
}

void JumpVariantScene::setup_and_add_model_board() {
    board.model->vertex_array = app->res.vertex_array["board_wood"_h];
    board.model->index_buffer = app->res.index_buffer["board_wood"_h];
    board.model->scale = WORLD_SCALE;
    board.model->material = app->res.material_instance["board_wood"_h];
    board.model->bounding_box = std::make_optional<Renderer::BoundingBox>();
    board.model->bounding_box->id = identifier::null;
    board.model->bounding_box->size = BOARD_BOUNDING_BOX;
    board.model->bounding_box->sort = false;
    board.model->cast_shadow = true;

    app->renderer->add_model(board.model);

    DEB_DEBUG("Setup model board");
}

void JumpVariantScene::setup_and_add_model_board_paint() {
    board.paint_model->vertex_array = app->res.vertex_array["board_paint"_h];
    board.paint_model->index_buffer = app->res.index_buffer["board_paint"_h];
    board.paint_model->position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model->scale = WORLD_SCALE;
    board.paint_model->material = app->res.material_instance["board_paint"_h];

    app->renderer->add_model(board.paint_model);

    DEB_DEBUG("Setup model board paint");
}

void JumpVariantScene::setup_and_add_model_pieces() {
    for (size_t i = 0; i < 9; i++) {
        setup_and_add_model_piece(
            i, WHITE_PIECE_POSITION(i),
            app->res.index_buffer["white_piece"_h]
        );
    }

    for (size_t i = 9; i < 18; i++) {
        setup_and_add_model_piece(
            i, BLACK_PIECE_POSITION(i),
            app->res.index_buffer["black_piece"_h]
        );
    }
}

void JumpVariantScene::setup_and_add_model_piece(size_t index, const glm::vec3& position,
        std::shared_ptr<gl::IndexBuffer> index_buffer) {
    auto& data = app->user_data<Data>();

    Piece& piece = board.pieces.at(index);

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->vertex_array = app->res.vertex_array[hs {"piece" + std::to_string(index)}];
    piece.model->index_buffer = index_buffer;
    piece.model->scale = WORLD_SCALE;
    piece.model->material = app->res.material_instance[hs {"piece" + std::to_string(index)}];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->bounding_box = std::make_optional<Renderer::BoundingBox>();
    piece.model->bounding_box->id = data.piece_ids[index];
    piece.model->bounding_box->size = PIECE_BOUNDING_BOX;
    piece.model->cast_shadow = true;

    app->renderer->add_model(piece.model);

    piece.source->set_position(position);
    piece.source->set_reference_distance(6.0f);

    DEB_DEBUG("Setup model piece {}", index);
}

void JumpVariantScene::setup_and_add_model_nodes() {
    for (size_t i = 0; i < 24; i++) {
        setup_and_add_model_node(i, NODE_POSITIONS[i]);
    }
}

void JumpVariantScene::setup_and_add_model_node(size_t index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    Node& node = board.nodes.at(index);

    node.model->vertex_array = app->res.vertex_array[hs {"node" + std::to_string(index)}];
    node.model->index_buffer = app->res.index_buffer["node"_h];
    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->material = app->res.material_instance[hs {"node" + std::to_string(index)}];
    node.model->bounding_box = std::make_optional<Renderer::BoundingBox>();
    node.model->bounding_box->id = data.node_ids[index];
    node.model->bounding_box->size = NODE_BOUNDING_BOX;

    app->renderer->add_model(node.model);

    DEB_DEBUG("Setup model node {}", index);
}

void JumpVariantScene::setup_entities() {
    board = JumpBoard {};
    board.model = app->res.model.load("board"_h);
    board.paint_model = app->res.model.load("board_paint"_h);

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i] = Piece {
            i, PieceType::White,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
    }

    for (size_t i = 9; i < 18; i++) {
        board.pieces[i] = Piece {
            i, PieceType::Black,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
    }

    for (size_t i = 0; i < 24; i++) {
        board.nodes[i] = Node {
            i, app->res.model.load(hs {"node" + std::to_string(i)})
        };
    }

    DEB_DEBUG("Setup entities");
}

void JumpVariantScene::setup_widgets() {
    auto& data = app->user_data<Data>();

    constexpr int LOWEST_RESOLUTION = 288;
    constexpr int HIGHEST_RESOLUTION = 1035;

    auto turn_indicator = app->res.image.load(
        "turn_indicator"_h, app->res.texture["white_indicator"_h]
    );
    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right);
    turn_indicator->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    app->gui_renderer->add_widget(turn_indicator);

    auto timer_text = app->res.text.load(
        "timer_text"_h,
        app->res.font["open_sans"_h],
        "00:00",
        1.5f,
        glm::vec3(0.9f)
    );
    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!data.options.hide_timer) {
        app->gui_renderer->add_widget(timer_text);
    }

    auto wait_indicator = app->res.image.load(
        "wait_indicator"_h, app->res.texture["wait_indicator"_h]
    );
    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right);
    wait_indicator->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    auto computer_thinking_indicator = app->res.image.load(
        "computer_thinking_indicator"_h, app->res.texture["computer_thinking_indicator"_h]
    );
    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right);
    computer_thinking_indicator->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
}

void JumpVariantScene::update_game_state() {
    switch (game.state) {
        case GameState::MaybeNextPlayer:
            switch (board.turn) {
                case BoardPlayer::White:
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
                    break;
                case BoardPlayer::Black:
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
                    break;
            }
            break;
        case GameState::HumanBeginMove:
            game.begin_human_move();
            game.state = GameState::HumanThinkingMove;
            break;
        case GameState::HumanThinkingMove:
            break;
        case GameState::HumanDoingMove:
            if (board.next_move) {
                game.state = GameState::HumanEndMove;
            }
            break;
        case GameState::HumanEndMove:
            game.end_human_move();
            game.state = GameState::MaybeNextPlayer;
            break;
        case GameState::ComputerBeginMove:
            game.begin_computer_move();
            game.state = GameState::ComputerThinkingMove;
            break;
        case GameState::ComputerThinkingMove:
            if (!minimax_thread.is_running()) {
                minimax_thread.join();

                const bool switched_turn = game.end_computer_move();

                update_after_computer_move(switched_turn);
            }
            break;
        case GameState::ComputerDoingMove:
            if (board.next_move) {
                game.state = GameState::ComputerEndMove;
            }
            break;
        case GameState::ComputerEndMove:
            game.state = GameState::MaybeNextPlayer;
            break;
    }
}

void JumpVariantScene::update_timer_text() {
    char time[32];
    timer.get_time_formatted(time);
    app->res.text["timer_text"_h]->set_text(time);
}

void JumpVariantScene::update_turn_indicator() {
    if (board.turn == BoardPlayer::White) {
        app->res.image["turn_indicator"_h]->set_image(app->res.texture["white_indicator"_h]);
    } else {
        app->res.image["turn_indicator"_h]->set_image(app->res.texture["black_indicator"_h]);
    }
}

void JumpVariantScene::update_wait_indicator() {
    if (!board.next_move) {
        if (!show_wait_indicator) {
            app->gui_renderer->add_widget(app->res.image["wait_indicator"_h]);
            show_wait_indicator = true;
        }
    } else {
        if (show_wait_indicator) {
            app->gui_renderer->remove_widget(app->res.image["wait_indicator"_h]);
            show_wait_indicator = false;
        }
    }
}

void JumpVariantScene::update_computer_thinking_indicator() {
    if (game.state == GameState::ComputerThinkingMove) {
        if (!show_computer_thinking_indicator) {
            app->gui_renderer->add_widget(app->res.image["computer_thinking_indicator"_h]);
            show_computer_thinking_indicator = true;
        }
    } else {
        if (show_computer_thinking_indicator) {
            app->gui_renderer->remove_widget(app->res.image["computer_thinking_indicator"_h]);
            show_computer_thinking_indicator = false;
        }
    }
}

void JumpVariantScene::update_after_human_move(bool did_action, bool switched_turn, bool must_take_piece_or_took_piece) {
    if (did_action) {
        game.state = GameState::HumanDoingMove;
    }

    if (did_action && !made_first_move && !timer.is_running()) {
        timer.start();
        made_first_move = true;
    }

    if (board.phase == BoardPhase::GameOver) {
        timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator();
    }

    if (must_take_piece_or_took_piece) {
        update_cursor(app, *this);
    }

    imgui_layer.can_undo = undo_redo_state.undo.size() > 0;
    imgui_layer.can_redo = undo_redo_state.redo.size() > 0;
}

void JumpVariantScene::update_after_computer_move(bool switched_turn) {
    game.state = GameState::ComputerDoingMove;

    if (!made_first_move && !timer.is_running()) {
        timer.start();
        made_first_move = true;
    }

    if (board.phase == BoardPhase::GameOver) {
        timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator();
    }

    imgui_layer.can_undo = undo_redo_state.undo.size() > 0;
    imgui_layer.can_redo = undo_redo_state.redo.size() > 0;
}

void JumpVariantScene::save_game() {
    board.finalize_pieces_state();

    JumpBoardSerialized serialized;
    board.to_serialized(serialized);

    save_load::SavedGame<JumpBoardSerialized> saved_game;
    saved_game.board_serialized = serialized;
    saved_game.camera_controller = camera_controller;
    saved_game.time = timer.get_time();

    time_t current;
    time(&current);
    saved_game.date = ctime(&current);

    saved_game.undo_redo_state = undo_redo_state;
    saved_game.white_player = game.white_player;
    saved_game.black_player = game.black_player;

    try {
        save_load::save_game_to_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not save game: {}", e.what());

        save_load::handle_save_file_not_open_error();
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not save game: {}", e.what());
    }
}

void JumpVariantScene::load_game() {
    board.finalize_pieces_state();

    save_load::SavedGame<JumpBoardSerialized> saved_game;

    try {
        save_load::load_game_from_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not load game: {}", e.what());

        save_load::handle_save_file_not_open_error();

        imgui_layer.show_could_not_load_game = true;
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not load game: {}", e.what());  // TODO maybe delete file

        imgui_layer.show_could_not_load_game = true;
        return;
    }

    board.from_serialized(saved_game.board_serialized);
    camera_controller = saved_game.camera_controller;
    timer = Timer {app, saved_game.time};
    undo_redo_state = std::move(saved_game.undo_redo_state);
    game.white_player = saved_game.white_player;
    game.black_player = saved_game.black_player;

    // Set camera pointer lost in serialization
    camera_controller.set_camera(&camera);

    made_first_move = false;

    update_cursor(app, *this);
    update_turn_indicator();
}


void JumpVariantScene::undo() {
    ASSERT(!undo_redo_state.undo.empty(), "Undo history must not be empty");

    if (!board.next_move) {
        DEB_WARNING("Cannot undo when pieces are in air");
        return;
    }

    const bool undo_game_over = board.phase == BoardPhase::None;

    using State = UndoRedoState<JumpBoardSerialized>::State;

    JumpBoardSerialized serialized;
    board.to_serialized(serialized);

    State current_state = { serialized, camera_controller };
    const State& previous_state = undo_redo_state.undo.back();

    board.from_serialized(previous_state.board_serialized);
    camera_controller = previous_state.camera_controller;

    undo_redo_state.undo.pop_back();
    undo_redo_state.redo.push_back(current_state);

    DEB_DEBUG("Undid move; popped from undo stack and pushed onto redo stack");

    game.state = GameState::MaybeNextPlayer;
    made_first_move = board.not_placed_white_pieces_count + board.not_placed_black_pieces_count != 18;

    if (undo_game_over) {
        timer.start();
    }

    update_cursor(app, *this);
    update_turn_indicator();
}

void JumpVariantScene::redo() {
    ASSERT(!undo_redo_state.redo.empty(), "Redo history must not be empty");

    if (!board.next_move) {
        DEB_WARNING("Cannot redo when pieces are in air");
        return;
    }

    using State = UndoRedoState<JumpBoardSerialized>::State;

    JumpBoardSerialized serialized;
    board.to_serialized(serialized);

    State current_state = { serialized, camera_controller };
    const State& previous_state = undo_redo_state.redo.back();

    board.from_serialized(previous_state.board_serialized);
    camera_controller = previous_state.camera_controller;

    undo_redo_state.redo.pop_back();
    undo_redo_state.undo.push_back(current_state);

    DEB_DEBUG("Redid move; popped from redo stack and pushed onto undo stack");

    game.state = GameState::MaybeNextPlayer;
    made_first_move = board.not_placed_white_pieces_count + board.not_placed_black_pieces_count != 18;

    const bool redo_game_over = board.phase == BoardPhase::None;

    if (redo_game_over) {
        timer.stop();
        board.phase = BoardPhase::GameOver;  // Make the game over screen show up again
    }

    update_cursor(app, *this);
    update_turn_indicator();
}
