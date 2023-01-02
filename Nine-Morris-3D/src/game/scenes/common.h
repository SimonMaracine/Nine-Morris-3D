#pragma once

#include <engine/engine_application.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/entities/piece.h"
#include "game/entities/node.h"
#include "game/point_camera_controller.h"
#include "other/data.h"
#include "other/constants.h"

void initialize_piece(Application* app, size_t index, std::shared_ptr<gl::Texture> diffuse_texture);
void initialize_piece_no_normal(Application* app, size_t index, std::shared_ptr<gl::Texture> diffuse_texture);

void change_skybox(Application* app);
void change_board_paint_texture(Application* app);

void setup_and_add_turn_indicator(Application* app);
void setup_and_add_timer_text(Application* app);
void setup_wait_indicator(Application* app);
void setup_computer_thinking_indicator(Application* app);

// Must be called only once
void initialize_game(Application* app);

template<typename S>
void setup_and_add_model_board(Application* app, S* scene) {
    auto& board = scene->board;

    board.model->scale = WORLD_SCALE;
    board.model->vertex_array = app->res.vertex_array["board_wood"_h];
    board.model->index_buffer = app->res.index_buffer["board_wood"_h];
    board.model->material = app->res.material_instance["board_wood"_h];
    board.model->cast_shadow = true;
    board.model->bounding_box = std::make_optional<Renderer::BoundingBox>();
    board.model->bounding_box->id = identifier::null;
    board.model->bounding_box->size = BOARD_BOUNDING_BOX;
    board.model->bounding_box->sort = false;

    app->renderer->add_model(board.model);

    DEB_DEBUG("Setup model board");
}

template<typename S>
void setup_and_add_model_board_paint(Application* app, S* scene) {
    const auto& board = scene->board;

    board.paint_model->position = glm::vec3(0.0f, PAINT_Y_POSITION, 0.0f);
    board.paint_model->scale = WORLD_SCALE;
    board.paint_model->vertex_array = app->res.vertex_array["board_paint"_h];
    board.paint_model->index_buffer = app->res.index_buffer["board_paint"_h];
    board.paint_model->material = app->res.material_instance["board_paint"_h];

    app->renderer->add_model(board.paint_model);

    DEB_DEBUG("Setup model board paint");
}

template<typename S>
void setup_and_add_model_piece(Application* app, S* scene, size_t index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    const Piece& piece = scene->board.pieces.at(index);

    const auto id = piece.type == PieceType::White ? "white_piece"_h : "black_piece"_h;

    piece.model->position = position;
    piece.model->rotation = RANDOM_PIECE_ROTATION();
    piece.model->scale = WORLD_SCALE;
    piece.model->vertex_array = app->res.vertex_array[id];
    piece.model->index_buffer = app->res.index_buffer[id];
    piece.model->material = app->res.material_instance[hs {"piece" + std::to_string(index)}];
    piece.model->outline_color = std::make_optional<glm::vec3>(1.0f);
    piece.model->cast_shadow = true;
    piece.model->bounding_box = std::make_optional<Renderer::BoundingBox>();
    piece.model->bounding_box->id = data.piece_ids[index];
    piece.model->bounding_box->size = PIECE_BOUNDING_BOX;

    app->renderer->add_model(piece.model);

    piece.source->set_position(position);
    piece.source->set_reference_distance(6.0f);

    DEB_DEBUG("Setup model piece {}", index);
}

template<typename S>
void setup_and_add_model_nodes(Application* app, S* scene) {
    for (size_t i = 0; i < MAX_NODES; i++) {
        setup_and_add_model_node(app, scene, i, NODE_POSITIONS[i]);
    }
}

template<typename S>
void setup_and_add_model_node(Application* app, S* scene, size_t index, const glm::vec3& position) {
    auto& data = app->user_data<Data>();

    const Node& node = scene->board.nodes.at(index);

    node.model->position = position;
    node.model->scale = WORLD_SCALE;
    node.model->vertex_array = app->res.vertex_array["node"_h];
    node.model->index_buffer = app->res.index_buffer["node"_h];
    node.model->material = app->res.material_instance[hs {"node" + std::to_string(index)}];
    node.model->bounding_box = std::make_optional<Renderer::BoundingBox>();
    node.model->bounding_box->id = data.node_ids[index];
    node.model->bounding_box->size = NODE_BOUNDING_BOX;

    app->renderer->add_model(node.model);

    DEB_DEBUG("Setup model node {}", index);
}

template<typename S>
void setup_camera(Application* app, S* scene) {
    auto& data = app->user_data<Data>();

    constexpr float PITCH = 47.0f;
    constexpr float DISTANCE_TO_POINT = 8.0f;

    scene->camera = Camera {};

    scene->camera_controller = PointCameraController {
        &scene->camera,
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

    scene->default_camera_position = scene->camera_controller.get_position();

    scene->camera_controller = PointCameraController {
        &scene->camera,
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

    app->renderer->set_camera_controller(&scene->camera_controller);
    update_listener(app, scene);

    DEB_DEBUG("Setup camera");
}

template<typename S>
void update_listener(Application* app, const S* scene) {
    app->openal->get_listener().set_position(scene->camera_controller.get_position());
    app->openal->get_listener().set_look_at_and_up(
        scene->camera_controller.get_point() - scene->camera_controller.get_position(),
        glm::rotate(UP_VECTOR, scene->camera_controller.get_rotation().y, UP_VECTOR)
    );
}

template<typename S>
void update_cursor(Application* app, const S* scene) {
    auto& data = app->user_data<Data>();

    if (data.options.custom_cursor) {
        if (scene->board.must_take_piece) {
            app->window->set_cursor(data.cross_cursor);

            app->res.quad["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_cross"_h];
        } else {
            app->window->set_cursor(data.arrow_cursor);

            app->res.quad["keyboard_controls"_h]->texture = app->res.texture["keyboard_controls_default"_h];
        }
    }
}

template<typename S>
void set_skybox(Application* app, const S* scene, Skybox skybox) {
    if (skybox == Skybox::None) {
        app->renderer->set_skybox(nullptr);
        return;
    }

    auto& data = app->user_data<Data>();

    scene->skybox_loader->start_loading_thread(data.launcher_options.texture_quality, data.options.skybox);
}

template<typename S>
void set_board_paint_texture(Application* app, const S* scene) {
    auto& data = app->user_data<Data>();

    scene->board_paint_texture_loader->start_loading_thread(
        data.launcher_options.texture_quality,
        data.options.labeled_board
    );
}

template<typename S>
void update_turn_indicator(Application* app, const S* scene) {
    if (scene->board.turn == BoardPlayer::White) {
        app->res.image["turn_indicator"_h]->set_image(app->res.texture["white_indicator"_h]);
    } else {
        app->res.image["turn_indicator"_h]->set_image(app->res.texture["black_indicator"_h]);
    }
}

template<typename S>
void update_wait_indicator(Application* app, S* scene) {
    if (!scene->board.next_move) {
        if (!scene->show_wait_indicator) {
            app->gui_renderer->add_widget(app->res.image["wait_indicator"_h]);
            scene->show_wait_indicator = true;
        }
    } else {
        if (scene->show_wait_indicator) {
            app->gui_renderer->remove_widget(app->res.image["wait_indicator"_h]);
            scene->show_wait_indicator = false;
        }
    }
}

template<typename S>
void update_computer_thinking_indicator(Application* app, S* scene) {
    if (scene->game.state == GameState::ComputerThinkingMove) {
        if (!scene->show_computer_thinking_indicator) {
            app->gui_renderer->add_widget(app->res.image["computer_thinking_indicator"_h]);
            scene->show_computer_thinking_indicator = true;
        }
    } else {
        if (scene->show_computer_thinking_indicator) {
            app->gui_renderer->remove_widget(app->res.image["computer_thinking_indicator"_h]);
            scene->show_computer_thinking_indicator = false;
        }
    }
}

template<typename S>
void update_timer_text(Application* app, const S* scene) {
    char time[32];
    scene->timer.get_time_formatted(time);
    app->res.text["timer_text"_h]->set_text(time);
}

template<typename S>
void update_after_human_move(Application* app, S* scene, bool did_action, bool switched_turn, bool must_take_or_took_piece) {
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
        update_turn_indicator(app, scene);
    }

    if (must_take_or_took_piece) {
        update_cursor(app, scene);
    }

    scene->imgui_layer.can_undo = scene->undo_redo_state.undo.size() > 0;
    scene->imgui_layer.can_redo = scene->undo_redo_state.redo.size() > 0;
}

template<typename S>
void update_after_computer_move(Application* app, S* scene, bool switched_turn) {
    scene->game.state = GameState::ComputerDoingMove;

    if (!scene->made_first_move && !scene->timer.is_running()) {
        scene->timer.start();
        scene->made_first_move = true;
    }

    if (scene->board.phase == BoardPhase::GameOver) {
        scene->timer.stop();
    }

    if (switched_turn) {
        update_turn_indicator(app, scene);
    }

    scene->imgui_layer.can_undo = scene->undo_redo_state.undo.size() > 0;
    scene->imgui_layer.can_redo = scene->undo_redo_state.redo.size() > 0;
}

template<typename S>
void update_game_state(Application* app, S* scene) {
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

                update_after_computer_move(app, scene, switched_turn);
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

template<typename S>
void setup_piece_on_node(Application* app, S* scene, size_t index, size_t node_index) {
    setup_and_add_model_piece(
        app,
        scene,
        index,
        PIECE_INDEX_POSITION(node_index)
    );

    scene->board.pieces.at(index).node_index = node_index;
    scene->board.nodes.at(node_index).piece_index = index;
}
