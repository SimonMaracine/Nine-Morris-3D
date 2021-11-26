#pragma once

#include <vector>

#include "opengl/renderer/camera.h"
#include "nine_morris_3d/board.h"

// #include <entt/entt.hpp>

namespace save_load {
    struct GameState {
        Board board;
        Camera camera;
        std::vector<Board> board_state_history;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board, camera, board_state_history);
        }
    };

    // Entities gather_entities(entt::entity board, entt::entity camera, entt::entity* nodes,
    //                          entt::entity* pieces);
    // void reset_entities(const Entities& entities, entt::entity* board, entt::entity* camera,
    //                     entt::entity* nodes, entt::entity* pieces);

    void save_game(const GameState& game_state);
    void load_game(GameState& game_state);

    bool save_files_exist();
}
