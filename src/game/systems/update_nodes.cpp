#include <entt/entt.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/components/board.h"
#include "game/components/node.h"
#include "game/systems/update_nodes.h"

#include "game/constants.h"

void UpdateNodesSystem::run() {
    const hover::Id hovered_id = app->renderer->get_hovered_id();

    auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto view = registry.view<NodeComponent, HoverComponent, ModelComponent>();

    for (auto entity : view) {
        auto [hover_c, model_c] = view.get<HoverComponent, ModelComponent>(entity);

        const bool hovered = hover_c.id == hovered_id;
        const bool highlight = (
            board_c.phase == BoardPhase::PlacePieces || board_c.phase == BoardPhase::MovePieces && board_c.selected_piece != entt::null
        );
        const bool permitted = !board_c.should_take_piece && board_c.is_players_turn;

        if (hovered && highlight && permitted) {
            model_c.model.material->set_vec4("u_color", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
        } else {
            model_c.model.material->set_vec4("u_color", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}
