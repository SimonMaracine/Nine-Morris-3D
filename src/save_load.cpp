#include <glm/glm.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "application/platform.h"
// #include "graphics/renderer/renderer.h"
// #include "graphics/renderer/camera.h"
// #include "graphics/renderer/material.h"
#include "save_load.h"
#include "nine_morris_3d.h"
// #include "undo_redo.h"
// #include "board.h"
// #include "piece.h"
// #include "node.h"
// #include "ecs/internal_components.h"
#include "game/components/piece_move.h"
#include "game/components/piece.h"
#include "game/components/node.h"
#include "game/components/camera_player_position.h"
#include "game/components/active.h"
#include "game/components/undo_redo_state.h"
#include "game/components/board.h"
// #include "other/paths.h"
// #include "other/logging.h"
// #include "other/user_data.h"

template<typename Archive>
void serialize(Archive& archive, TransformComponent& transform_c) {
    archive(transform_c.position, transform_c.rotation, transform_c.scale);
}

/*
Unserialized variables:
    vertex_array, material
*/
template<typename Archive>
void serialize(Archive& archive, ModelComponent& model_c) {
    archive(model_c.index_count);
}

/*
Unserialized variables:
    texture
*/
template<typename Archive>
void serialize(Archive& archive, QuadComponent&) {
    archive();
}

template<typename Archive>
void serialize(Archive& archive, RenderComponent&) {  // TODO might not work...
    archive();
}

template<typename Archive>
void serialize(Archive& archive, GuiImageComponent& gui_image_c) {
    archive(gui_image_c.image);
}

template<typename Archive>
void serialize(Archive& archive, GuiTextComponent& gui_text_c) {
    archive(gui_text_c.text);
}

/*
Unserialized variables:
    gui_renderer
*/
template<typename Archive>
void serialize(Archive& archive, GuiRenderComponent&) {  // TODO might not work...
    archive();
}

template<typename Archive>
void serialize(Archive& archive, GuiOffsetComponent& gui_offset_c) {
    archive(gui_offset_c.offsets);
}

template<typename Archive>
void serialize(Archive& archive, GuiStickComponent& gui_stick_c) {
    archive(gui_stick_c.sticky);
}

template<typename Archive>
void serialize(Archive& archive, GuiScaleComponent& gui_scale_c) {
    archive(gui_scale_c.min_scale, gui_scale_c.max_scale, gui_scale_c.min_bound, gui_scale_c.max_bound);
}

template<typename Archive>
void serialize(Archive& archive, CameraComponent& camera_c) {
    archive(camera_c.camera);
}

template<typename Archive>
void serialize(Archive& archive, HoverComponent& hover_c) {  // TODO might not work...
    archive(hover_c.id);
}

template<typename Archive>
void serialize(Archive& archive, Camera& camera) {
    archive(
        camera.sensitivity, camera.position, camera.pitch, camera.yaw, camera.view_matrix,
        camera.projection_matrix, camera.projection_view_matrix, camera.point,
        camera.distance_to_point, camera.angle_around_point, camera.x_velocity,
        camera.y_velocity, camera.zoom_velocity, camera.auto_move_x, camera.target_angle_around_point,
        camera.auto_x_velocity, camera.virtual_angle_around_point, camera.auto_move_y,
        camera.target_pitch, camera.auto_y_velocity, camera.virtual_pitch, camera.auto_move_zoom,
        camera.target_distance_to_point, camera.auto_zoom_velocity, camera.virtual_distance_to_point,
        camera.cached_towards_position, camera.dont_auto_call_go_towards_position
    );
}

template<typename Archive>
void serialize(Archive& archive, OutlineComponent& outline_c) {
    archive(outline_c.outline_enabled, outline_c.outline_color);
}

template<typename Archive>
void serialize(Archive& archive, BoardComponent& board_c) {
    archive(
        board_c.nodes, board_c.pieces, board_c.phase, board_c.turn,
        board_c.ending, board_c.ending_message, board_c.white_pieces_count, board_c.black_pieces_count,
        board_c.not_placed_pieces_count, board_c.should_take_piece, board_c.hovered_node,
        board_c.hovered_piece, board_c.selected_piece, board_c.can_jump, board_c.turns_without_mills,
        board_c.repetition_history, board_c.state_history, board_c.keyboard, board_c.game_context,
        board_c.next_move, board_c.is_players_turn, board_c.switched_turn
    );
}

template<typename Archive>
void serialize(Archive& archive, ThreefoldRepetitionHistory& repetition_history) {
    archive(repetition_history.ones, repetition_history.twos);
}

template<typename Archive>
void serialize(Archive& archive, ThreefoldRepetitionHistory::PositionPlusInfo& position_plus_info) {
    archive(position_plus_info.position, position_plus_info.piece_index, position_plus_info.node_index);
}

template<typename Archive>
void serialize(Archive& archive, UndoRedoStateComponent& undo_redo_state_c) {
    archive(undo_redo_state_c.undo, undo_redo_state_c.redo);
}

template<typename Archive>
void serialize(Archive& archive, UndoRedoStateComponent::Page& page) {
    archive(page.board_c, page.camera_c, page.game_state);
}

template<typename Archive>
void serialize(Archive& archive, PieceComponent& piece_c) {
    archive(
        piece_c.type, piece_c.in_use, piece_c.node_index, piece_c.node, piece_c.index,
        piece_c.show_outline, piece_c.to_take, piece_c.pending_remove, piece_c.selected
    );
}

template<typename Archive>
void serialize(Archive& archive, PieceMoveComponent& piece_move_c) {
    archive(
        piece_move_c.type, piece_move_c.velocity, piece_move_c.target, piece_move_c.target0,
        piece_move_c.target1, piece_move_c.reached_target0, piece_move_c.reached_target1,
        piece_move_c.moving
    );
}

template<typename Archive>
void serialize(Archive& archive, NodeComponent& node_c) {
    archive(node_c.piece_index, node_c.piece, node_c.index);
}

template<typename Archive>
void serialize(Archive& archive, CameraPlayerPosition& camera_player_position_c) {
    archive(camera_player_position_c.white_position, camera_player_position_c.black_position);
}

template<typename Archive>
void serialize(Archive& archive, ActiveComponent& active_c) {
    static_cast<void>(active_c);
    archive();
}

namespace glm {
    template<typename Archive>
    void serialize(Archive& archive, vec3& vector) {
        archive(vector.x, vector.y, vector.z);
    }

    template<typename Archive>
    void serialize(Archive& archive, mat4& matrix) {
        archive(
            matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
            matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
            matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
            matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]
        );
    }
}

namespace save_load {
    template<typename Archive>
    void serialize(Archive& archive, SavedGame& saved_game) {
        archive(
            saved_game.board_c, saved_game.camera_c, saved_game.time, saved_game.date,
            saved_game.undo_redo_state_c, saved_game.camera_player_position_c,
            saved_game.white_player, saved_game.black_player
        );
    }

    void save_game_to_file(const SavedGame& saved_game) noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_saved_data(SAVE_GAME_FILE);
        } catch (const user_data::UserNameError& e) {
            throw SaveFileError(e.what());
        }

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file '" + std::string(SAVE_GAME_FILE) + "' for writing"
            );
        }

        try {
            cereal::BinaryOutputArchive output {file};
            output(saved_game);
        } catch (const std::exception& e) {  // Just to be sure...
            throw SaveFileError(e.what());
        }

        DEB_INFO("Saved game to file '{}'", SAVE_GAME_FILE);
    }

    void load_game_from_file(SavedGame& saved_game) noexcept(false) {
        std::string file_path;
        try {
            file_path = paths::path_for_saved_data(SAVE_GAME_FILE);
        } catch (const user_data::UserNameError& e) {
            throw SaveFileError(e.what());
        }

        std::ifstream file {file_path, std::ios::binary};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file '" + std::string(SAVE_GAME_FILE) + "'"
            );
        }

        try {
            cereal::BinaryInputArchive input {file};
            input(saved_game);
        } catch (const cereal::Exception& e) {
            throw SaveFileError(e.what());
        } catch (const std::exception& e) {
            throw SaveFileError(e.what());
        }

        DEB_INFO("Loaded game from file '{}'", SAVE_GAME_FILE);
    }

    void delete_save_game_file(std::string_view file_path) {
        if (remove(file_path.data()) != 0) {
            REL_INFO("Could not delete save game file '{}'", file_path);
        } else {
            REL_INFO("Deleted save game file '{}'", file_path);
        }
    }

    void handle_save_file_not_open_error(std::string_view app_name) {
        bool user_data_directory;

        try {
            user_data_directory = user_data::user_data_directory_exists(app_name);
        } catch (const user_data::UserNameError& e) {
            REL_ERROR("{}", e.what());
            return;
        }

        if (!user_data_directory) {
            REL_INFO("User data folder missing; creating one...");

            try {
                const bool success = user_data::create_user_data_directory(app_name);
                if (!success) {
                    REL_ERROR("Could not create user data directory");
                    return;
                }
            } catch (const user_data::UserNameError& e) {
                REL_ERROR("{}", e.what());
                return;
            }
        }
    }
}
