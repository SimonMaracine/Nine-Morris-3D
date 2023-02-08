#include <engine/engine_audio.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/boards/jump_board.h"
#include "game/entities/serialization/jump_board_serialized.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/scenes/jump_plus_variant_scene.h"
#include "game/scenes/imgui_layer.h"
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

    objects.add<object::Quad>("keyboard_controls"_H);  // TODO clean up
    // auto keyboard_controls = scene.quad.load("keyboard_controls"_H);

    initialize_pieces();
    setup_entities();

    setup_and_add_model_board();
    setup_and_add_model_board_paint();
    setup_and_add_model_nodes();
    setup_and_add_model_pieces();

    setup_and_add_turn_indicator();
    setup_and_add_timer_text();
    setup_wait_indicator();
    setup_computer_thinking_indicator();
    setup_camera();

    update_turn_indicator();

    keyboard = KeyboardControls {app, &board, objects.get<object::Quad>("keyboard_controls"_H)};
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
    board.scene = this;
    board.keyboard = &keyboard;
    board.camera_controller = &camera_controller;
    board.undo_redo_state = &undo_redo_state;

    app->window->set_cursor(data.options.custom_cursor ? data.arrow_cursor : 0);

#ifdef NM3D_PLATFORM_DEBUG
    app->renderer->origin = true;
    scene_list.add(objects.get<object::Quad>("light_bulb"_H));
    // app->renderer->add_quad(scene.quad["light_bulb"_H].get());
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
    // app->renderer->clear();  // FIXME this is replaced
    // app->gui_renderer->clear();

    // Should dispose of these
    release_piece_material_instances();

    made_first_move = false;

    skybox_loader->join();
    board_paint_texture_loader->join();

    app->evt.remove_events(this);
}

void JumpPlusVariantScene::on_awake() {
    imgui_layer = ImGuiLayer<JumpPlusVariantScene, JumpBoardSerialized> {app, this};
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
    update_listener();

    update_game_state();
    update_timer_text();
    update_wait_indicator();
    update_computer_thinking_indicator();

    skybox_loader->update(app);
    board_paint_texture_loader->update(app);
}

void JumpPlusVariantScene::on_fixed_update() {
    camera_controller.update_friction();
}

void JumpPlusVariantScene::on_imgui_update() {
    update_all_imgui();
}

void JumpPlusVariantScene::on_mouse_button_pressed(const MouseButtonPressedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::Left) {
        if (board.next_move && board.phase != BoardPhase::None) {
            board.click(app->renderer->get_hovered_id());
        }
    }
}

void JumpPlusVariantScene::on_mouse_button_released(const MouseButtonReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.button == input::MouseButton::Left) {
        const bool valid_phases = board.phase == BoardPhase::MovePieces;

        if (board.next_move && board.is_players_turn && valid_phases) {
            const auto flags = board.release(app->renderer->get_hovered_id());

            update_after_human_move(
                flags.did_action, flags.switched_turn, flags.must_take_or_took_piece
            );
        }

        if (show_keyboard_controls) {
            scene_list.remove(objects.get<object::Quad>("keyboard_controls"_H));
            // app->renderer->remove_quad(scene.quad["keyboard_controls"_H].get());
            show_keyboard_controls = false;
        }
    }
}

void JumpPlusVariantScene::on_key_pressed(const KeyPressedEvent& event) {
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
                scene_list.add(objects.get<object::Quad>("keyboard_controls"_H));
                // app->renderer->add_quad(scene.quad["keyboard_controls"_H].get());
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
            const bool valid_phases = board.phase == BoardPhase::MovePieces;

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

void JumpPlusVariantScene::on_key_released(const KeyReleasedEvent& event) {
    if (imgui_layer.hovering_gui) {
        return;
    }

    if (event.key == input::Key::Space) {
        camera_controller.go_towards_position(default_camera_position);
    }
}

void JumpPlusVariantScene::on_window_resized(const WindowResizedEvent& event) {
    if (event.width == 0 || event.height == 0) {
        return;
    }

    camera.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
}

void JumpPlusVariantScene::setup_and_add_model_pieces() {
    Index index = 0;

    // White pieces
    setup_piece_on_node(index++, 0);
    setup_piece_on_node(index++, 3);
    setup_piece_on_node(index++, 6);
    setup_piece_on_node(index++, 17);
    setup_piece_on_node(index++, 20);
    setup_piece_on_node(index++, 23);

    // Black pieces
    setup_piece_on_node(index++, 2);
    setup_piece_on_node(index++, 5);
    setup_piece_on_node(index++, 8);
    setup_piece_on_node(index++, 15);
    setup_piece_on_node(index++, 18);
    setup_piece_on_node(index++, 21);
}

void JumpPlusVariantScene::initialize_pieces() {
    auto& data = app->user_data<Data>();

    if (data.launcher_options.normal_mapping) {
        for (size_t i = 0; i < 6; i++) {
            initialize_piece(i, app->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 6; i < 12; i++) {
            initialize_piece(i, app->res.texture["black_piece_diffuse"_H]);
        }
    } else {
        for (size_t i = 0; i < 6; i++) {
            initialize_piece_no_normal(i, app->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 6; i < 12; i++) {
            initialize_piece_no_normal(i, app->res.texture["black_piece_diffuse"_H]);
        }
    }
}

void JumpPlusVariantScene::setup_entities() {
    board = JumpBoard {};
    board.model = objects.add<object::Model>("board"_H);
    board.paint_model = objects.add<object::Model>("board_paint"_H);
    // board.model = scene.model.load("board"_H).get();
    // board.paint_model = scene.model.load("board_paint"_H).get();

    board.phase = BoardPhase::MovePieces;

    for (size_t i = 0; i < 6; i++) {
        Piece piece = Piece {
            static_cast<Index>(i),
            PieceType::White,
            objects.get<object::Model>(hs("piece" + std::to_string(i))),
            // scene.model.load(hs("piece" + std::to_string(i))).get(),
            app->res.al_source.load(hs("piece" + std::to_string(i)))
        };
        piece.in_use = true;

        board.pieces[i] = piece;
    }

    for (size_t i = 6; i < 12; i++) {
        Piece piece = Piece {
            static_cast<Index>(i),
            PieceType::Black,
            objects.get<object::Model>(hs("piece" + std::to_string(i))),
            // scene.model.load(hs("piece" + std::to_string(i))).get(),
            app->res.al_source.load(hs("piece" + std::to_string(i)))
        };
        piece.in_use = true;

        board.pieces[i] = piece;
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        board.nodes[i] = Node {
            static_cast<Index>(i),
            objects.get<object::Model>(hs("node" + std::to_string(i)))
            // scene.model.load(hs("node" + std::to_string(i))).get()
        };
    }

    // Set white pieces to show outline
    board.update_piece_outlines();

    DEB_DEBUG("Setup entities");
}

void JumpPlusVariantScene::imgui_draw_debug() {
    ImGui::Text("Turns without mills: %u", board.turns_without_mills);
}
