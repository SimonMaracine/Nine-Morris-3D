#pragma once

#include <engine/engine_other.h>

#include "other/constants.h"

template<typename S, typename B>
void generic_update_menubar(S* scene) {
    save_load::SavedGame<B> saved_game;
    try {
        save_load::load_game_from_file(saved_game, scene->save_game_file_name);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not load game: {}", e.what());
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not load game: {}", e.what());  // TODO maybe delete file
    }
    scene->last_save_game_date = std::move(saved_game.date);
    DEB_INFO("Checked last saved game");

    scene->info_file_path = file_system::path_for_logs(logging::get_info_file());
    scene->save_game_file_path = file_system::path_for_saved_data(scene->save_game_file_name);
}

template<typename S, typename B>
void generic_save_game(S* scene) {
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
void generic_load_game(S* scene) {
    scene->board.finalize_pieces_state();

    save_load::SavedGame<B> saved_game;

    try {
        save_load::load_game_from_file(saved_game, scene->save_game_file_name);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not load game: {}", e.what());

        save_load::handle_save_file_not_open_error();

        scene->window = WindowImGui::ShowCouldNotLoadGame;
        return;
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not load game: {}", e.what());  // TODO maybe delete file

        scene->window = WindowImGui::ShowCouldNotLoadGame;
        return;
    }

    scene->board.from_serialized(saved_game.board_serialized);
    scene->camera_controller = saved_game.camera_controller;
    scene->timer = Timer {scene->app, saved_game.time};
    scene->undo_redo_state = std::move(saved_game.undo_redo_state);
    scene->game.white_player = saved_game.white_player;
    scene->game.black_player = saved_game.black_player;

    // Set camera pointer lost in serialization
    scene->camera_controller.set_camera(&scene->camera);

    scene->made_first_move = false;

    scene->update_cursor();
    scene->update_turn_indicator();
}

template<typename S, typename B>
void generic_undo(S* scene) {
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

    // Set camera pointer lost in serialization
    scene->camera_controller.set_camera(&scene->camera);

    scene->undo_redo_state.undo.pop_back();
    scene->undo_redo_state.redo.push_back(current_state);

    DEB_DEBUG("Undid move; popped from undo stack and pushed onto redo stack");

    scene->game.state = GameState::MaybeNextPlayer;
    scene->made_first_move = scene->board.turn_count != 0;

    if (undo_game_over) {
        scene->timer.start();
    }

    scene->update_cursor();
    scene->update_turn_indicator();
}

template<typename S, typename B>
void generic_redo(S* scene) {
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

    // Set camera pointer lost in serialization
    scene->camera_controller.set_camera(&scene->camera);

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

    scene->update_cursor();
    scene->update_turn_indicator();
}
