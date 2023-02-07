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

    setup_and_add_turn_indicator(app, this);
    setup_and_add_timer_text(app, this);
    setup_wait_indicator(app, this);
    setup_computer_thinking_indicator(app, this);
    setup_camera(app, this);

    update_turn_indicator(app, this);

    keyboard = KeyboardControls {app, &board, scene.quad["keyboard_controls"_H].get()};
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
    app->renderer->add_quad(scene.quad["light_bulb"_H].get());
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

    // Should dispose of these
    release_piece_material_instances(app);

    made_first_move = false;

    skybox_loader->join();
    board_paint_texture_loader->join();

    app->evt.remove_events(this);
}

void StandardGameScene::on_awake() {
    imgui_layer = ImGuiLayer<StandardGameScene, StandardBoardSerialized> {app, this};
    save_game_file_name = save_load::save_game_file_name(get_name());

    skybox_loader = std::make_unique<assets_load::SkyboxLoader>(
        [this]() {
            change_skybox(app, this);
        }
    );
    board_paint_texture_loader = std::make_unique<assets_load::BoardPaintTextureLoader>(
        [this]() {
            change_board_paint_texture(app);
        }
    );
}

void StandardGameScene::on_update() {
    if (!imgui_layer.hovering_gui) {
        camera_controller.update_controls(app->get_delta());
        board.update_nodes(app->renderer->get_hovered_id());
        board.update_pieces(app->renderer->get_hovered_id());
    } else {
        camera_controller.discard_events(app);
    }

    camera_controller.update_camera(app->get_delta());
    board.move_pieces();
    timer.update();

    // Update listener position, look at and up vectors every frame
    update_listener(app, this);

    update_game_state(app, this);
    update_timer_text(this);
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

    if (event.button == input::MouseButton::Left) {
        if (board.next_move && board.phase != BoardPhase::None) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void StandardGameScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::Left) {
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
            app->renderer->remove_quad(scene.quad["keyboard_controls"_H].get());
            show_keyboard_controls = false;
        }
    }
}

void StandardGameScene::on_key_pressed(const KeyPressedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    switch (event.key) {
        case input::Key::Up:
        case input::Key::Down:
        case input::Key::Left:
        case input::Key::Right:
        case input::Key::Enter:
            if (!show_keyboard_controls) {
                app->renderer->add_quad(scene.quad["keyboard_controls"_H].get());
                show_keyboard_controls = true;
                return;
            }
            break;
        default:
            break;
    }

    using KB = KeyboardControls;

    switch (event.key) {
        case input::Key::Up:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Up, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::Down:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Down, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::Left:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Left, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::Right:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Right, camera_controller.get_angle_around_point()
                )
            );
            break;
        case input::Key::Enter: {
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

    if (event.key == input::Key::Space) {
        camera_controller.go_towards_position(default_camera_position);
    }
}

void StandardGameScene::on_window_resized(const WindowResizedEvent& event) {
    if (event.width == 0 || event.height == 0) {
        return;
    }

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
    board.model = scene.model.load("board"_H).get();
    board.paint_model = scene.model.load("board_paint"_H).get();

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i] = Piece {
            static_cast<Index>(i),
            PieceType::White,
            scene.model.load(hs("piece" + std::to_string(i))).get(),
            app->res.al_source.load(hs("piece" + std::to_string(i)))
        };
    }

    for (size_t i = 9; i < 18; i++) {
        board.pieces[i] = Piece {
            static_cast<Index>(i),
            PieceType::Black,
            scene.model.load(hs("piece" + std::to_string(i))).get(),
            app->res.al_source.load(hs("piece" + std::to_string(i)))
        };
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        board.nodes[i] = Node {
            static_cast<Index>(i),
            scene.model.load(hs("node" + std::to_string(i))).get()
        };
    }

    DEB_DEBUG("Setup entities");
}

void StandardGameScene::initialize_pieces() {
    auto& data = app->user_data<Data>();

    if (data.launcher_options.normal_mapping) {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece(app, i, app->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece(app, i, app->res.texture["black_piece_diffuse"_H]);
        }
    } else {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece_no_normal(app, i, app->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece_no_normal(app, i, app->res.texture["black_piece_diffuse"_H]);
        }
    }
}

void StandardGameScene::save_game() {
    save_game_generic<StandardGameScene, StandardBoardSerialized>(this);
}

void StandardGameScene::load_game() {
    load_game_generic<StandardGameScene, StandardBoardSerialized>(app, this);
}

void StandardGameScene::undo() {
    undo_generic<StandardGameScene, StandardBoardSerialized>(app, this);
}

void StandardGameScene::redo() {
    redo_generic<StandardGameScene, StandardBoardSerialized>(app, this);
}

void StandardGameScene::imgui_draw_debug() {
    ImGui::Text("White and black pieces: %u, %u", board.white_pieces_count, board.black_pieces_count);
    ImGui::Text("Not placed pieces: %u, %u", board.not_placed_white_pieces_count, board.not_placed_black_pieces_count);
    ImGui::Text("Can jump: %s, %s", board.can_jump[0] ? "true" : "false", board.can_jump[1] ? "true" : "false");
    ImGui::Text("Turns without mills: %u", board.turns_without_mills);
}
