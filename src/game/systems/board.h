#pragma once

#include <entt/entt.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

// #include "application/application.h"
// #include "ecs/system.h"

// void set_game_context(GameContext* game_context) { this->game_context = game_context; }

// bool place_piece_system(entt::registry& registry, hover::Id hovered_id);
// bool take_piece_system(entt::registry& registry, hover::Id hovered_id);
bool put_down_piece_system(entt::registry& registry, hover::Id hovered_id);
void computer_place_piece_system(entt::registry& registry, size_t node_index);
void computer_take_piece_system(entt::registry& registry, size_t node_index);
void computer_put_down_piece_system(entt::registry& registry, size_t source_node_index, size_t destination_node_index);
// void move_pieces_system(entt::registry& registry, float dt);
void select_piece_system(entt::registry& registry, hover::Id hovered_id);
void press_system(entt::registry& registry, hover::Id hovered_id);
void release_system(entt::registry& registry);
void finalize_pieces_state_system(entt::registry& registry);
// void update_cursor_system(entt::registry& registry);
// void update_nodes_system(entt::registry& registry, hover::Id hovered_id);
// void update_pieces_system(entt::registry& registry, hover::Id hovered_id);

// void copy_smart(BoardComponent& destination, const BoardComponent& source, StateHistory* history);
