#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/boards/jump_board.h"
#include "game/entities/serialization/jump_board_serialized.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/scenes/jump_plus_variant_scene.h"
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

void JumpPlusVariantScene::on_start() {
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

    app->evt.add_event<MouseButtonPressedEvent, &JumpPlusVariantScene::on_mouse_button_pressed>(this);
    app->evt.add_event<MouseButtonReleasedEvent, &JumpPlusVariantScene::on_mouse_button_released>(this);
    app->evt.add_event<KeyPressedEvent, &JumpPlusVariantScene::on_key_pressed>(this);
    app->evt.add_event<KeyReleasedEvent, &JumpPlusVariantScene::on_key_released>(this);
    app->evt.add_event<WindowResizedEvent, &JumpPlusVariantScene::on_window_resized>(this);
}

void JumpPlusVariantScene::on_stop() {
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

    if (skybox_loader->joinable()) {
        skybox_loader->join();
    }

    if (board_paint_texture_loader->joinable()) {
        board_paint_texture_loader->join();
    }

    app->evt.remove_events(this);
}

void JumpPlusVariantScene::on_awake() {
    imgui_layer = ImGuiLayer<JumpPlusVariantScene, JumpBoardSerialized> {app, this};
    save_game_file_name = save_load::save_game_file_name(get_name());

    skybox_loader = std::make_unique<assets_load::SkyboxLoader>(
        assets_load::skybox, std::bind(change_skybox, app)
    );
    board_paint_texture_loader = std::make_unique<assets_load::BoardPaintTextureLoader>(
        assets_load::board_paint_texture, std::bind(change_board_paint_texture, app)
    );
}

void JumpPlusVariantScene::on_update() {
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
    update_timer_text(app, this);
    update_wait_indicator(app, this);
    update_computer_thinking_indicator(app, this);

    skybox_loader->update(app);
    board_paint_texture_loader->update(app);
}

void JumpPlusVariantScene::on_fixed_update() {
    camera_controller.update_friction();
}

void JumpPlusVariantScene::on_imgui_update() {
    update_all_imgui(app, this);
}

void JumpPlusVariantScene::on_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::LEFT) {
        if (board.next_move && board.phase != BoardPhase::None) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void JumpPlusVariantScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::LEFT) {
        const bool valid_phases = board.phase == BoardPhase::MovePieces;

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

void JumpPlusVariantScene::on_key_pressed(const KeyPressedEvent& event) {
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
            break;
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
            const bool valid_phases = board.phase == BoardPhase::MovePieces;

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

void JumpPlusVariantScene::on_key_released(const KeyReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.key == input::Key::SPACE) {
        camera_controller.go_towards_position(default_camera_position);
    }
}

void JumpPlusVariantScene::on_window_resized(const WindowResizedEvent& event) {
    camera.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
}

void JumpPlusVariantScene::setup_and_add_model_pieces() {
    Index index = 0;

    // White pieces
    setup_piece_on_node(app, this, index++, 0);
    setup_piece_on_node(app, this, index++, 3);
    setup_piece_on_node(app, this, index++, 6);
    setup_piece_on_node(app, this, index++, 17);
    setup_piece_on_node(app, this, index++, 20);
    setup_piece_on_node(app, this, index++, 23);

    // Black pieces
    setup_piece_on_node(app, this, index++, 2);
    setup_piece_on_node(app, this, index++, 5);
    setup_piece_on_node(app, this, index++, 8);
    setup_piece_on_node(app, this, index++, 15);
    setup_piece_on_node(app, this, index++, 18);
    setup_piece_on_node(app, this, index++, 21);
}

void JumpPlusVariantScene::initialize_pieces() {
    auto& data = app->user_data<Data>();

    if (data.launcher_options.normal_mapping) {
        for (size_t i = 0; i < 6; i++) {
            initialize_piece(app, i, app->res.texture["white_piece_diffuse"_h]);
        }

        for (size_t i = 6; i < 12; i++) {
            initialize_piece(app, i, app->res.texture["black_piece_diffuse"_h]);
        }
    } else {
        for (size_t i = 0; i < 6; i++) {
            initialize_piece_no_normal(app, i, app->res.texture["white_piece_diffuse"_h]);
        }

        for (size_t i = 6; i < 12; i++) {
            initialize_piece_no_normal(app, i, app->res.texture["black_piece_diffuse"_h]);
        }
    }
}

void JumpPlusVariantScene::setup_entities() {
    board = JumpBoard {};
    board.model = app->res.model.load("board"_h);
    board.paint_model = app->res.model.load("board_paint"_h);

    board.phase = BoardPhase::MovePieces;

    for (size_t i = 0; i < 6; i++) {
        Piece piece = Piece {
            static_cast<Index>(i),
            PieceType::White,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
        piece.in_use = true;

        board.pieces[i] = piece;
    }

    for (size_t i = 6; i < 12; i++) {
        Piece piece = Piece {
            static_cast<Index>(i),
            PieceType::Black,
            app->res.model.load(hs {"piece" + std::to_string(i)}),
            app->res.al_source.load(hs {"piece" + std::to_string(i)})
        };
        piece.in_use = true;

        board.pieces[i] = piece;
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        board.nodes[i] = Node {
            static_cast<Index>(i),
            app->res.model.load(hs {"node" + std::to_string(i)})
        };
    }

    // Set white pieces to show outline
    board.update_piece_outlines();

    DEB_DEBUG("Setup entities");
}

void JumpPlusVariantScene::save_game() {
    save_game_generic<JumpPlusVariantScene, JumpBoardSerialized>(this);
}

void JumpPlusVariantScene::load_game() {
    load_game_generic<JumpPlusVariantScene, JumpBoardSerialized>(app, this);
}

void JumpPlusVariantScene::undo() {
    undo_generic<JumpPlusVariantScene, JumpBoardSerialized>(app, this);
}

void JumpPlusVariantScene::redo() {
    redo_generic<JumpPlusVariantScene, JumpBoardSerialized>(app, this);
}

void JumpPlusVariantScene::imgui_draw_debug() {
    ImGui::Text("Turns without mills: %u", board.turns_without_mills);
}
