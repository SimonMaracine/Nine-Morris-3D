#include <engine/public/application_base.h>
#include <engine/public/audio.h>
#include <engine/public/graphics.h>
#include <engine/public/other.h>
#include <engine/public/external/resmanager.h++>

#include "game/entities/boards/standard_board.h"
#include "game/entities/serialization/standard_board_serialized.h"
#include "game/entities/board.h"
#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/minimax/standard_game/minimax_standard_game.h"
#include "game/scenes/standard_game_scene.h"
#include "game/scenes/common.h"
#include "game/game_options.h"
#include "game/save_load.h"
#include "game/assets.h"
#include "game/assets_load.h"
#include "other/constants.h"
#include "other/data.h"
#include "other/options_gracefully.h"

void StandardGameScene::on_start() {
    auto& data = ctx->data<Data>();

    initialize_renderables();
    initialize_pieces();
    setup_entities();

    setup_and_add_model_board();
    setup_and_add_model_board_paint();
    setup_and_add_model_nodes();
    setup_and_add_model_pieces();

#ifndef NM3D_PLATFORM_DISTRIBUTION
    setup_light_bulb();
#endif
    setup_and_add_turn_indicator();
    setup_and_add_timer_text();
    setup_wait_indicator();
    setup_computer_thinking_indicator();
    setup_camera();

    update_turn_indicator();

    keyboard = KeyboardControls {ctx, &board, objects.get<sm::renderables::Quad>("keyboard_controls"_H)};
    keyboard.post_initialize();

    undo_redo_state = UndoRedoState<StandardBoardSerialized> {};

    minimax_thread = MinimaxThread {&board};

    game = GameContext {
        static_cast<GamePlayer>(data.options.white_player),
        static_cast<GamePlayer>(data.options.black_player),
        &board,
        &minimax_thread,
        &minimax_algorithm
    };

    timer = Timer {ctx};

    board.ctx = ctx;
    board.scene = this;
    board.keyboard = &keyboard;
    board.camera_controller = &camera_controller;
    board.undo_redo_state = &undo_redo_state;

    ctx->window->set_cursor(data.options.custom_cursor ? "arrow"_H : "null"_H);

#ifndef NM3D_PLATFORM_DISTRIBUTION
    ctx->r3d->origin = true;
    scene_list.add(objects.get<sm::renderables::Quad>("light_bulb"_H));
#endif

    update_menubar();

    camera_controller.go_towards_position(default_camera_position);
    camera_controller.connect_events(ctx);

    // Can dispose of these
    ctx->res.texture_data.clear();
    ctx->res.sound_data.clear();

    ctx->evt.connect<sm::MouseButtonPressedEvent, &StandardGameScene::on_mouse_button_pressed>(this);
    ctx->evt.connect<sm::MouseButtonReleasedEvent, &StandardGameScene::on_mouse_button_released>(this);
    ctx->evt.connect<sm::KeyPressedEvent, &StandardGameScene::on_key_pressed>(this);
    ctx->evt.connect<sm::KeyReleasedEvent, &StandardGameScene::on_key_released>(this);
    ctx->evt.connect<sm::WindowResizedEvent, &StandardGameScene::on_window_resized>(this);
}

void StandardGameScene::on_stop() {
    auto& data = ctx->data<Data>();

    options_gracefully::save_to_file<game_options::GameOptions>(
        game_options::GAME_OPTIONS_FILE, data.options
    );

    if (data.options.save_on_exit && !ctx->running && made_first_move) {
        save_game();
    }

#ifndef NM3D_PLATFORM_DISTRIBUTION
    ctx->r3d->origin = false;
#endif

    imgui_reset();
    camera_controller.disconnect_events(ctx);

    // Should dispose of these
    release_piece_material_instances();

    made_first_move = false;

    skybox_loader->join();
    board_paint_texture_loader->join();

    ctx->evt.disconnect(this);
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
    camera_controller.update_controls(ctx->delta);
    camera_controller.update_camera(ctx->delta);

    board.update_nodes(ctx->r3d->get_hovered_id());
    board.update_pieces(ctx->r3d->get_hovered_id());
    board.move_pieces();

    timer.update();

    // Update listener position, look at and up vectors every frame
    update_listener();

    update_game_state();
    update_timer_text();
    update_wait_indicator();
    update_computer_thinking_indicator();

    skybox_loader->update(ctx);
    board_paint_texture_loader->update(ctx);
}

void StandardGameScene::on_fixed_update() {
    camera_controller.update_friction();
}

void StandardGameScene::on_imgui_update() {
    update_all_imgui();
}

void StandardGameScene::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        if (board.next_move && board.phase != BoardPhase::None) {
            board.click(ctx->r3d->get_hovered_id());
        }
    }
}

void StandardGameScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        const bool valid_phases = (
            board.phase == BoardPhase::PlacePieces || board.phase == BoardPhase::MovePieces
        );

        if (board.next_move && board.is_players_turn && valid_phases) {
            const auto flags = board.release(ctx->r3d->get_hovered_id());

            update_after_human_move(
                flags.did_action, flags.switched_turn, flags.must_take_or_took_piece
            );
        }

        if (show_keyboard_controls) {
            scene_list.remove(objects.get<sm::renderables::Quad>("keyboard_controls"_H));
            show_keyboard_controls = false;
        }
    }
}

void StandardGameScene::on_key_pressed(const sm::KeyPressedEvent& event) {
    switch (event.key) {
        case sm::Key::Up:
        case sm::Key::Down:
        case sm::Key::Left:
        case sm::Key::Right:
        case sm::Key::Enter:
            if (!show_keyboard_controls) {
                scene_list.add(objects.get<sm::renderables::Quad>("keyboard_controls"_H));
                show_keyboard_controls = true;
                return;
            }
            break;
        default:
            break;
    }

    using KB = KeyboardControls;

    switch (event.key) {
        case sm::Key::Up:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Up, camera_controller.get_angle_around_point()
                )
            );
            break;
        case sm::Key::Down:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Down, camera_controller.get_angle_around_point()
                )
            );
            break;
        case sm::Key::Left:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Left, camera_controller.get_angle_around_point()
                )
            );
            break;
        case sm::Key::Right:
            keyboard.move(
                KB::calculate(
                    KB::Direction::Right, camera_controller.get_angle_around_point()
                )
            );
            break;
        case sm::Key::Enter: {
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

void StandardGameScene::on_key_released(const sm::KeyReleasedEvent& event) {
    if (event.key == sm::Key::Space) {
        camera_controller.go_towards_position(default_camera_position);
    }
}

void StandardGameScene::on_window_resized(const sm::WindowResizedEvent& event) {
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
    board.model = objects.get<sm::renderables::Model>("board"_H);
    board.paint_model = objects.get<sm::renderables::Model>("board_paint"_H);

    for (size_t i = 0; i < 9; i++) {
        board.pieces[i] = Piece {
            i,
            PieceType::White,
            objects.get<sm::renderables::Model>(hs("piece" + std::to_string(i))),
            ctx->res.al_source.load(hs("piece" + std::to_string(i)))
        };
    }

    for (size_t i = 9; i < 18; i++) {
        board.pieces[i] = Piece {
            i,
            PieceType::Black,
            objects.get<sm::renderables::Model>(hs("piece" + std::to_string(i))),
            ctx->res.al_source.load(hs("piece" + std::to_string(i)))
        };
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        board.nodes[i] = Node {
            i,
            objects.get<sm::renderables::Model>(hs("node" + std::to_string(i)))
        };
    }

    LOG_DEBUG("Setup entities");
}

void StandardGameScene::initialize_renderables() {
    board.model = objects.add<sm::renderables::Model>("board"_H);
    board.paint_model = objects.add<sm::renderables::Model>("board_paint"_H);

    for (size_t i = 0; i < 9; i++) {
        objects.add<sm::renderables::Model>(hs("piece" + std::to_string(i)));
    }

    for (size_t i = 9; i < 18; i++) {
        objects.add<sm::renderables::Model>(hs("piece" + std::to_string(i)));
    }

    for (size_t i = 0; i < MAX_NODES; i++) {
        objects.add<sm::renderables::Model>(hs("node" + std::to_string(i)));
    }

    objects.add<sm::renderables::Quad>("keyboard_controls"_H);
}

void StandardGameScene::initialize_pieces() {
    auto& data = ctx->data<Data>();

    if (data.launcher_options.normal_mapping) {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece(i, ctx->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece(i, ctx->res.texture["black_piece_diffuse"_H]);
        }
    } else {
        for (size_t i = 0; i < 9; i++) {
            initialize_piece_no_normal(i, ctx->res.texture["white_piece_diffuse"_H]);
        }

        for (size_t i = 9; i < 18; i++) {
            initialize_piece_no_normal(i, ctx->res.texture["black_piece_diffuse"_H]);
        }
    }
}

void StandardGameScene::draw_debug_imgui() {
    ImGui::Text("White and black pieces: %u, %u", board.white_pieces_on_board_count, board.black_pieces_on_board_count);
    ImGui::Text("Not placed pieces: %u, %u", board.white_pieces_outside_count, board.black_pieces_outside_count);
    ImGui::Text("Can jump: %s, %s", board.can_jump[0] ? "true" : "false", board.can_jump[1] ? "true" : "false");
    ImGui::Text("Turns without mills: %u", board.turns_without_mills);
}

void StandardGameScene::draw_ai_configuration_imgui() {
    imgui_draw_window("Computer AI Configuration", [this]() {
        int piece = minimax_algorithm.parameters.PIECE;
        int freedom = minimax_algorithm.parameters.FREEDOM;
        int depth = static_cast<int>(minimax_algorithm.parameters.DEPTH);

        if (ImGui::InputInt("Piece", &piece)) {
            if (piece >= 0 && piece <= 10) {
                minimax_algorithm.parameters.PIECE = piece;
            }
        }

        if (ImGui::InputInt("Freedom", &freedom)) {
            if (freedom >= 0 && freedom <= 10) {
                minimax_algorithm.parameters.FREEDOM = freedom;
            }
        }

        if (ImGui::InputInt("Depth", &depth)) {
            if (depth > 0 && depth <= 10) {
                minimax_algorithm.parameters.DEPTH = static_cast<unsigned int>(depth);
            }
        }
    });
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
