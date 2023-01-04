#include <engine/engine_application.h>
#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/scenes/standard_game_scene.h"
#include "game/scenes/imgui_layer.h"
#include "game/scenes/common.h"
#include "game/game_options.h"
#include "game/save_load.h"
#include "game/assets.h"
#include "game/assets_load.h"
#include "other/constants.h"
#include "other/data.h"
#include "other/options_gracefully.h"

using namespace encrypt;

void StandardGameScene::on_start() {
    auto& data = app->user_data<Data>();

    initialize_pieces();
    setup_entities();

    setup_and_add_model_board(app, this);
    setup_and_add_model_board_paint(app, this);
    setup_and_add_model_nodes(app, this);
    setup_and_add_model_pieces();

    setup_and_add_turn_indicator(app);
    setup_and_add_timer_text(app);
    setup_wait_indicator(app);
    setup_computer_thinking_indicator(app);
    setup_camera(app, this);

    update_turn_indicator(app, this);

    keyboard = KeyboardControls {app, &board, app->res.quad["keyboard_controls"_h]};
    keyboard.post_initialize();

    undo_redo_state = UndoRedoState<StandardBoardSerialized> {};

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

    app->evt.add_event<MouseButtonPressedEvent, &StandardGameScene::on_mouse_button_pressed>(this);
    app->evt.add_event<MouseButtonReleasedEvent, &StandardGameScene::on_mouse_button_released>(this);
    app->evt.add_event<KeyPressedEvent, &StandardGameScene::on_key_pressed>(this);
    app->evt.add_event<KeyReleasedEvent, &StandardGameScene::on_key_released>(this);
    app->evt.add_event<WindowResizedEvent, &StandardGameScene::on_window_resized>(this);
}

void StandardGameScene::on_stop() {
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

    // Delete all piece material instances
    for (size_t i = 0; i < MAX_PIECES; i++) {
        app->res.material_instance.release(hs {"piece" + std::to_string(i)});
    }

    made_first_move = false;

    if (skybox_loader->joinable()) {
        skybox_loader->join();
    }

    if (board_paint_texture_loader->joinable()) {
        board_paint_texture_loader->join();
    }

    app->evt.remove_events(this);
}

void StandardGameScene::on_awake() {
    imgui_layer = ImGuiLayer<StandardGameScene, StandardBoardSerialized> {app, this};
    save_game_file_name = save_load::save_game_file_name(get_name());

    skybox_loader = std::make_unique<assets_load::SkyboxLoader>(
        assets_load::skybox, std::bind(change_skybox, app)
    );
    board_paint_texture_loader = std::make_unique<assets_load::BoardPaintTextureLoader>(
        assets_load::board_paint_texture, std::bind(change_board_paint_texture, app)
    );
}

void StandardGameScene::on_update() {
    if (!imgui_layer.hovering_gui) {
        camera_controller.update_controls(app->get_delta());
        board.update_nodes(app->renderer->get_hovered_id());
        board.update_pieces(app->renderer->get_hovered_id());
    }

    camera_controller.update_camera(app->get_delta());
    board.move_pieces();
    timer.update();

    // Update listener position, look at and up vectors every frame
    update_listener(app, this);

    update_game_state(app, this);
    update_timer_text(app, this);
    update_wait_indicator(app, this);
    update_computer_thinking_indicator(app, this);

    skybox_loader->update(app);
    board_paint_texture_loader->update(app);
}

void StandardGameScene::on_fixed_update() {
    camera_controller.update_friction();
}

void StandardGameScene::on_imgui_update() {
    update_all_imgui(app, this);
}

void StandardGameScene::on_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move && board.phase != BoardPhase::None) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void StandardGameScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
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
                app, this, flags.did_action, flags.switched_turn, flags.must_take_or_took_piece
            );
        }

        if (show_keyboard_controls) {
            app->renderer->remove_quad(app->res.quad["keyboard_controls"_h]);
            show_keyboard_controls = false;
        }
    }
}

void StandardGameScene::on_key_pressed(const KeyPressedEvent& event) {
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
                    app, this, flags.did_action, flags.switched_turn, flags.must_take_or_took_piece
                );
            }
            break;
        }
        default:
            break;
    }
}

void StandardGameScene::on_key_released(const KeyReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.key == input::Key::SPACE) {
        camera_controller.go_towards_position(default_camera_position);
    }
}

void StandardGameScene::on_window_resized(const WindowResizedEvent& event) {
    camera.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
}

void StandardGameScene::setup_and_add_model_pieces() {
    for (size_t i = 0; i < 9; i++) {
        setup_and_add_model_piece(
            app,
            this,
            i,
            WHITE_PIECE_POSITION(i)
        );
    }

    for (size_t i = 9; i < 18; i++) {
        setup_and_add_model_piece(
            app,
            this,
            i,
            BLACK_PIECE_POSITION(i)
        );
    }
}

void StandardGameScene::setup_entities() {
    board = StandardBoard {};
    board.model = app->res.model.load("board"_h);
    board.paint_model = app->res.model.load("board_paint"_h);

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i] = Piece {
            static_cast<Index>(i),
            PieceType::White,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
    }

    for (size_t i = 9; i < 18; i++) {
        board.pieces[i] = Piece {
            static_cast<Index>(i),
            PieceType::Black,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        board.nodes[i] = Node {
            static_cast<Index>(i),
            app->res.model.load(hs {"node" + std::to_string(i)})
        };
    }

    DEB_DEBUG("Setup entities");
}

void StandardGameScene::initialize_pieces() {
    auto& data = app->user_data<Data>();

    if (data.launcher_options.normal_mapping) {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece(app, i, app->res.texture["white_piece_diffuse"_h]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece(app, i, app->res.texture["black_piece_diffuse"_h]);
        }
    } else {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece_no_normal(app, i, app->res.texture["white_piece_diffuse"_h]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece_no_normal(app, i, app->res.texture["black_piece_diffuse"_h]);
        }
    }
}

void StandardGameScene::save_game() {
    board.finalize_pieces_state();

    StandardBoardSerialized serialized;
    board.to_serialized(serialized);

    save_load::SavedGame<StandardBoardSerialized> saved_game;
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
        save_load::save_game_to_file(saved_game, save_game_file_name);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not save game: {}", e.what());

        save_load::handle_save_file_not_open_error();
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not save game: {}", e.what());
    }
}

void StandardGameScene::load_game() {
    board.finalize_pieces_state();

    save_load::SavedGame<StandardBoardSerialized> saved_game;

    try {
        save_load::load_game_from_file(saved_game, save_game_file_name);
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

    update_cursor(app, this);
    update_turn_indicator(app, this);
}

void StandardGameScene::undo() {
    ASSERT(!undo_redo_state.undo.empty(), "Undo history must not be empty");

    if (!board.next_move) {
        DEB_WARNING("Cannot undo when pieces are in air");
        return;
    }

    const bool undo_game_over = board.phase == BoardPhase::None;

    using State = UndoRedoState<StandardBoardSerialized>::State;

    StandardBoardSerialized serialized;
    board.to_serialized(serialized);

    State current_state = { serialized, camera_controller };
    const State& previous_state = undo_redo_state.undo.back();

    board.from_serialized(previous_state.board_serialized);
    camera_controller = previous_state.camera_controller;

    undo_redo_state.undo.pop_back();
    undo_redo_state.redo.push_back(current_state);

    DEB_DEBUG("Undid move; popped from undo stack and pushed onto redo stack");

    game.state = GameState::MaybeNextPlayer;
    made_first_move = board.turn_count != 0;

    if (undo_game_over) {
        timer.start();
    }

    update_cursor(app, this);
    update_turn_indicator(app, this);
}

void StandardGameScene::redo() {
    ASSERT(!undo_redo_state.redo.empty(), "Redo history must not be empty");

    if (!board.next_move) {
        DEB_WARNING("Cannot redo when pieces are in air");
        return;
    }

    using State = UndoRedoState<StandardBoardSerialized>::State;

    StandardBoardSerialized serialized;
    board.to_serialized(serialized);

    State current_state = { serialized, camera_controller };
    const State& previous_state = undo_redo_state.redo.back();

    board.from_serialized(previous_state.board_serialized);
    camera_controller = previous_state.camera_controller;

    undo_redo_state.redo.pop_back();
    undo_redo_state.undo.push_back(current_state);

    DEB_DEBUG("Redid move; popped from redo stack and pushed onto undo stack");

    game.state = GameState::MaybeNextPlayer;
    made_first_move = board.turn_count != 0;

    const bool redo_game_over = board.phase == BoardPhase::None;

    if (redo_game_over) {
        timer.stop();
        board.phase = BoardPhase::GameOver;  // Make the game over screen show up again
    }

    update_cursor(app, this);
    update_turn_indicator(app, this);
}

void StandardGameScene::imgui_draw_debug() {
    ImGui::Text("White and black pieces: %u, %u", board.white_pieces_count, board.black_pieces_count);
    ImGui::Text("Not placed pieces: %u, %u", board.not_placed_white_pieces_count, board.not_placed_black_pieces_count);
    ImGui::Text("Can jump: %s, %s", board.can_jump[0] ? "true" : "false", board.can_jump[1] ? "true" : "false");
    ImGui::Text("Turns without mills: %u", board.turns_without_mills);
}
