#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "game/constants.h"

// format is the first argument to __VA_ARGS__
#define FORMATTED_MESSAGE(result, size, ...) \
    char result[size]; \
    sprintf(result, __VA_ARGS__);

#define TURN_IS_WHITE_SO(_true, _false) (board_c.turn == BoardPlayer::White ? _true : _false)
#define WAIT_FOR_NEXT_MOVE() board_c.next_move = false
#define CAN_MAKE_MOVE() board_c.next_move = true

constexpr unsigned int WINDMILLS[16][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

entt::entity new_piece_to_place(entt::registry& registry, PieceType type, float x_pos, float z_pos, entt::entity node);
void take_and_raise_piece(entt::registry& registry, entt::entity piece);
void set_pieces_show_outline(entt::registry& registry, PieceType type, bool show);
void game_over(entt::registry& registry, BoardEnding ending, PieceType type_to_hide, std::string_view ending_message);
void switch_turn(entt::registry& registry);
bool is_windmill_made(entt::registry& registry, entt::entity node, PieceType type);
void set_pieces_to_take(entt::registry& registry, PieceType type, bool take);
size_t number_of_pieces_in_windmills(entt::registry& registry, PieceType type);
void unselect_other_pieces(entt::registry& registry, entt::entity currently_selected_piece);
void update_outlines(entt::registry& registry);
bool can_go(entt::registry& registry, entt::entity source_node, entt::entity destination_node);
void check_player_number_of_pieces(entt::registry& registry, BoardPlayer player);
bool is_player_blocked(entt::registry& registry, BoardPlayer player);
void remember_position_and_check_repetition(entt::registry& registry, entt::entity piece, entt::entity node);
void remember_state(entt::registry& registry);
void arrive_at_node(entt::registry& registry, entt::entity piece);
void prepare_piece_for_linear_move(entt::registry& registry, entt::entity piece, const glm::vec3& target, const glm::vec3& velocity);
void prepare_piece_for_three_step_move(entt::registry& registry, entt::entity piece, const glm::vec3& target,
        const glm::vec3& velocity, const glm::vec3& target0, const glm::vec3& target1);
GamePosition get_position(entt::registry& registry);
