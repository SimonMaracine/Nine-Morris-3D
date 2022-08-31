#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/components/piece.h"
#include "game/components/active.h"
#include "game/systems/update_pieces.h"

void UpdatePiecesSystem::run() {
    const hover::Id hovered_id = app->renderer->get_hovered_id();

    auto view = registry.view<PieceComponent, HoverComponent, ModelComponent, OutlineComponent, ActiveComponent>();

    for (auto entity : view) {
        auto [piece_c, hover_c, model_c, outline_c] = view.get<PieceComponent, HoverComponent, ModelComponent, OutlineComponent>(entity);

        if (piece_c.selected) {
            outline_c.outline_color = glm::vec3(1.0f, 0.0f, 0.0f);
        } else if (piece_c.show_outline && hover_c.id == hovered_id && piece_c.in_use && !piece_c.pending_remove) {
            outline_c.outline_color = glm::vec3(1.0f, 0.5f, 0.0f);
        } else if (piece_c.to_take && hover_c.id == hovered_id && piece_c.in_use) {
            model_c.model.material->set_vec3("u_material.tint", glm::vec3(1.0f, 0.2f, 0.2f));
        } else {
            model_c.model.material->set_vec3("u_material.tint", glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (piece_c.selected || piece_c.show_outline && hover_c.id == hovered_id && piece_c.in_use && !piece_c.pending_remove) {
            if (!outline_c.outline_enabled) {
                app->renderer->update_model(model_c.model, Renderer::WithOutline | Renderer::CastShadow);  // TODO use registry.patch + on_update
                outline_c.outline_enabled = true;
            }
        } else {
            if (outline_c.outline_enabled) {
                app->renderer->update_model(model_c.model, Renderer::CastShadow);
                outline_c.outline_enabled = false;
            }
        }
    }
}
