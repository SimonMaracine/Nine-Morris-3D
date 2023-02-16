#include <engine/engine_application.h>
#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/entities/board.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/scenes/standard_game_scene.h"
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

    initialize_renderables();
    initialize_pieces();
    setup_entities();

    setup_and_add_model_board();
    setup_and_add_model_board_paint();
    setup_and_add_model_nodes();
    setup_and_add_model_pieces();

#ifdef NM3D_PLATFORM_DEBUG
    setup_light_bulb();
#endif
    setup_and_add_turn_indicator();
    setup_and_add_timer_text();
    setup_wait_indicator();
    setup_computer_thinking_indicator();
    setup_camera();

    update_turn_indicator();

    keyboard = KeyboardControls {app, &board, objects.get<renderables::Quad>("keyboard_controls"_H)};
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
    board.scene = this;
    board.keyboard = &keyboard;
    board.camera_controller = &camera_controller;
    board.undo_redo_state = &undo_redo_state;

    app->window->set_cursor(data.options.custom_cursor ? data.arrow_cursor : 0);

#ifdef NM3D_PLATFORM_DEBUG
    app->renderer->origin = true;
    scene_list.add(objects.get<renderables::Quad>("light_bulb"_H));
#endif

    update_menubar();

    camera_controller.go_towards_position(default_camera_position);
    camera_controller.connect_events(app);  // TODO this

    // Can dispose of these
    app->res.texture_data.clear();
    app->res.sound_data.clear();

    app->evt.connect<MouseButtonPressedEvent, &StandardGameScene::on_mouse_button_pressed>(this);
    app->evt.connect<MouseButtonReleasedEvent, &StandardGameScene::on_mouse_button_released>(this);
    app->evt.connect<KeyPressedEvent, &StandardGameScene::on_key_pressed>(this);
    app->evt.connect<KeyReleasedEvent, &StandardGameScene::on_key_released>(this);
    app->evt.connect<WindowResizedEvent, &StandardGameScene::on_window_resized>(this);
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

    imgui_reset();
    camera_controller.disconnect_events(app);  // TODO this

    // Should dispose of these
    release_piece_material_instances();

    made_first_move = false;

    skybox_loader->join();
    board_paint_texture_loader->join();

    app->evt.disconnect(this);  // TODO this
}

void StandardGameScene::on_awake() {
    imgui_initialize();
    save_game_file_name = save_load::save_game_file_name(get_name());

    skybox_loader = std::make_unique<assets_load::SkyboxLoader>(
        [this]() {
            change_skybox();
        }
    );
    board_paint_texture_loader = std::make_unique<assets_load::BoardPaintTextureLoader>(
        [this]() {
            change_board_paint_texture();
        }
    );
}

void StandardGameScene::on_update() {
    if (!hovering_gui) {
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
    update_listener();

    update_game_state();
    update_timer_text();
    update_wait_indicator();
    update_computer_thinking_indicator();

    skybox_loader->update(app);
    board_paint_texture_loader->update(app);
}

void StandardGameScene::on_fixed_update() {
    camera_controller.update_friction();
}

void StandardGameScene::on_imgui_update() {
    update_all_imgui();
}

void StandardGameScene::on_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    if (hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::Left) {
        if (board.next_move && board.phase != BoardPhase::None) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void StandardGameScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    if (hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::Left) {
        const bool valid_phases = (
            board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces
        );

        if (board.next_move && board.is_players_turn && valid_phases) {
            const auto flags = board.release(app->renderer->get_hovered_id());

            update_after_human_move(
                flags.did_action, flags.switched_turn, flags.must_take_or_took_piece
            );
        }

        if (show_keyboard_controls) {
            scene_list.remove(objects.get<renderables::Quad>("keyboard_controls"_H));
            show_keyboard_controls = false;
        }
    }
}

void StandardGameScene::on_key_pressed(const KeyPressedEvent& event) {
    if (hovering_gui) {
        return;
    }

    switch (event.key) {
        case input::Key::Up:
        case input::Key::Down:
        case input::Key::Left:
        case input::Key::Right:
        case input::Key::Enter:
            if (!show_keyboard_controls) {
                scene_list.add(objects.get<renderables::Quad>("keyboard_controls"_H));
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
                    flags.did_action, flags.switched_turn, flags.must_take_or_took_piece
                );
            }
            break;
        }
        default:
            break;
    }
}

void StandardGameScene::on_key_released(const KeyReleasedEvent& event) {
    if (hovering_gui) {
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
            i, WHITE_PIECE_POSITION(i)
        );
    }

    for (size_t i = 9; i < 18; i++) {
        setup_and_add_model_piece(
            i, BLACK_PIECE_POSITION(i)
        );
    }
}

void StandardGameScene::setup_entities() {
    board = StandardBoard {};
    board.model = objects.get<renderables::Model>("board"_H);
    board.paint_model = objects.get<renderables::Model>("board_paint"_H);

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i] = Piece {
            i,
            PieceType::White,
            objects.get<renderables::Model>(hs("piece" + std::to_string(i))),
            app->res.al_source.load(hs("piece" + std::to_string(i)))
        };
    }

    for (size_t i = 9; i < 18; i++) {
        board.pieces[i] = Piece {
            i,
            PieceType::Black,
            objects.get<renderables::Model>(hs("piece" + std::to_string(i))),
            app->res.al_source.load(hs("piece" + std::to_string(i)))
        };
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        board.nodes[i] = Node {
            i,
            objects.get<renderables::Model>(hs("node" + std::to_string(i)))
        };
    }

    DEB_DEBUG("Setup entities");
}

void StandardGameScene::initialize_renderables() {
    board.model = objects.add<renderables::Model>("board"_H);
    board.paint_model = objects.add<renderables::Model>("board_paint"_H);

    for (size_t i = 0; i < 9; i++) {
        objects.add<renderables::Model>(hs("piece" + std::to_string(i)));
    }

    for (size_t i = 9; i < 18; i++) {
        objects.add<renderables::Model>(hs("piece" + std::to_string(i)));
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        objects.add<renderables::Model>(hs("node" + std::to_string(i)));
    }

    objects.add<renderables::Quad>("keyboard_controls"_H);
}

void StandardGameScene::initialize_pieces() {
    auto& data = app->user_data<Data>();

    if (data.launcher_options.normal_mapping) {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece(i, app->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece(i, app->res.texture["black_piece_diffuse"_H]);
        }
    } else {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece_no_normal(i, app->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece_no_normal(i, app->res.texture["black_piece_diffuse"_H]);
        }
    }
}

void StandardGameScene::draw_debug_imgui() {
    ImGui::Text("White and black pieces: %u, %u", board.white_pieces_count, board.black_pieces_count);
    ImGui::Text("Not placed pieces: %u, %u", board.not_placed_white_pieces_count, board.not_placed_black_pieces_count);
    ImGui::Text("Can jump: %s, %s", board.can_jump[0] ? "true" : "false", board.can_jump[1] ? "true" : "false");
    ImGui::Text("Turns without mills: %u", board.turns_without_mills);
}

void StandardGameScene::update_menubar() {
    generic_update_menubar<StandardGameScene, StandardBoardSerialized>(this);
}

void StandardGameScene::save_game() {
    generic_save_game<StandardGameScene, StandardBoardSerialized>(this);
}

void StandardGameScene::load_game() {
    generic_load_game<StandardGameScene, StandardBoardSerialized>(this);
}

void StandardGameScene::undo() {
    generic_undo<StandardGameScene, StandardBoardSerialized>(this);
}

void StandardGameScene::redo() {
    generic_redo<StandardGameScene, StandardBoardSerialized>(this);
}

Board& StandardGameScene::get_board() {
    return board;
}

size_t StandardGameScene::get_undo_size() {
    return undo_redo_state.undo.size();
}

size_t StandardGameScene::get_redo_size() {
    return undo_redo_state.redo.size();
}
