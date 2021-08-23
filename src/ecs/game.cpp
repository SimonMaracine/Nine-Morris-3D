#include <cassert>
#include <algorithm>
#include <vector>
#include <array>

#include <entt/entt.hpp>

#include "ecs/game.h"
#include "ecs/components.h"
#include "ecs/undo.h"
#include "application/input.h"
#include "other/logging.h"

constexpr int windmills[16][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

static entt::entity place_new_piece(entt::registry& registry, Piece type, float x_pos,
                                    float z_pos, entt::entity node_entity) {
    auto view = registry.view<TransformComponent, PieceComponent, MoveComponent>();

    for (entt::entity entity : view) {
        auto [transform, piece, move] = view.get(entity);

        if (!piece.active && piece.type == type) {
            move.target.x = x_pos;
            move.target.y = PIECE_Y_POSITION;
            move.target.z = z_pos;

            move.velocity = (move.target - transform.position) * PIECE_MOVE_SPEED;
            move.distance_to_travel = move.target - transform.position;
            move.should_move = true;

            piece.active = true;
            piece.node = node_entity;

            return entity;
        }
    }

    assert(false);
}

static void take_raise_piece(entt::registry& registry, entt::entity piece_entity) {
    auto [transform, piece, move] = registry.get<TransformComponent, PieceComponent,
                                                 MoveComponent>(piece_entity);

    move.target.x = transform.position.x;
    move.target.y = PIECE_Y_POSITION + 1.5f;
    move.target.z = transform.position.z;

    move.velocity = (move.target - transform.position) * PIECE_MOVE_SPEED;
    move.distance_to_travel = move.target - transform.position;
    move.should_move = true;

    piece.node = entt::null;
    piece.pending_remove = true;
}

static void set_pieces_show_outline(entt::registry& registry, Piece type, bool show) {
    auto view = registry.view<PieceComponent>();

    for (entt::entity entity : view) {
        auto& piece = view.get<PieceComponent>(entity);

        if (piece.type == type) {
            piece.show_outline = show;
        }
    }
}

static void game_over(entt::registry& registry, entt::entity board, Ending ending, Piece type_to_hide) {
    auto& state = STATE(board);

    state.phase = Phase::GameOver;
    state.ending = ending;
    set_pieces_show_outline(registry, type_to_hide, false);

    switch (ending) {
        case Ending::WinnerWhite:
            SPDLOG_INFO("Game over, white wins");
            break;
        case Ending::WinnerBlack:
            SPDLOG_INFO("Game over, black wins");
            break;
        case Ending::TieBetweenBothPlayers:
            SPDLOG_INFO("Game over, tie between both players");
            break;
        default:
            assert(false);
    }
}

static void switch_turn(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    if (state.phase == Phase::MovePieces) {
        state.turns_without_mills++;

        if (state.turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            game_over(registry, board, Ending::TieBetweenBothPlayers,
                state.turn == Player::White ? Piece::White : Piece::Black);
        }
    }

    if (state.turn == Player::White) {
        state.turn = Player::Black;
    } else {
        state.turn = Player::White;
    }
}

static void switch_turn_no_check(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    if (state.turn == Player::White) {
        state.turn = Player::Black;
    } else {
        state.turn = Player::White;
    }
}

static bool is_windmill_made(entt::registry& registry, entt::entity board,
                             entt::entity node, Piece type) {
    auto& state = STATE(board);

    for (int i = 0; i < 16; i++) {
        const int* mill = windmills[i];

        auto& node1 = NODE(state.nodes[mill[0]]);
        auto& node2 = NODE(state.nodes[mill[1]]);
        auto& node3 = NODE(state.nodes[mill[2]]);

        if (node1.piece != entt::null && node2.piece != entt::null &&
                node3.piece != entt::null) {
            auto& piece1 = PIECE(node1.piece);
            auto& piece2 = PIECE(node2.piece);
            auto& piece3 = PIECE(node3.piece);

            if (piece1.type == type && piece2.type == type && piece3.type == type) {
                if (piece1.node == node || piece2.node == node || piece3.node == node) {
                    return true;
                }
            }
        }
    }

    return false;
}

static void set_pieces_to_take(entt::registry& registry, Piece type, bool take) {
    auto view = registry.view<PieceComponent>();

    for (entt::entity entity : view) {
        auto& piece = view.get<PieceComponent>(entity);

        if (piece.type == type) {
            piece.to_take = take;
        }
    }
}

static int number_of_pieces_in_windmills(entt::registry& registry, entt::entity board, Piece type) {
    auto& state = STATE(board);

    std::vector<entt::entity> pieces_inside_mills;

    for (int i = 0; i < 16; i++) {
        const int* mill = windmills[i];

        auto& node1 = NODE(state.nodes[mill[0]]);
        auto& node2 = NODE(state.nodes[mill[1]]);
        auto& node3 = NODE(state.nodes[mill[2]]);

        if (node1.piece != entt::null && node2.piece != entt::null &&
                node3.piece != entt::null) {
            auto& piece1 = PIECE(node1.piece);
            auto& piece2 = PIECE(node2.piece);
            auto& piece3 = PIECE(node3.piece);

            if (piece1.type == type && piece2.type == type && piece3.type == type) {
                std::vector<entt::entity>::iterator it;

                it = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node1.piece);
                if (it == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node1.piece);
                }
                it = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node2.piece);
                if (it == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node2.piece);
                }
                it = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node3.piece);
                if (it == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node3.piece);
                }
            }
        }
    }

    SPDLOG_DEBUG("Number of {} pieces in mills: {}", type == Piece::White ? "white" : "black",
        pieces_inside_mills.size());

    return pieces_inside_mills.size();
}

static void unselect_other_pieces(entt::registry& registry, entt::entity currently_selected_piece) {
    auto view = registry.view<PieceComponent>();

    for (entt::entity entity : view) {
        auto& piece = view.get<PieceComponent>(entity);

        if (entity != currently_selected_piece) {
            piece.selected = false;
        }
    }
}

static void update_outlines(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    if (state.phase == Phase::MovePieces) {
        if (state.turn == Player::White) {
            set_pieces_show_outline(registry, Piece::White, true);
            set_pieces_show_outline(registry, Piece::Black, false);
        } else {
            set_pieces_show_outline(registry, Piece::Black, true);
            set_pieces_show_outline(registry, Piece::White, false);
        }
    }
}

static bool can_go(entt::registry& registry, entt::entity board, entt::entity source_node,
                   entt::entity destination_node) {
    auto& state = STATE(board);
    auto& source = NODE(source_node);
    auto& destination = NODE(destination_node);

    assert(source_node != destination_node);

    if (state.can_jump[(int) state.turn]) {
        return true;
    }

    switch (source.id) {
        case 0:
            if (destination.id == 1 || destination.id == 9)
                return true;
            break;
        case 1:
            if (destination.id == 0 || destination.id == 2 || destination.id == 4)
                return true;
            break;
        case 2:
            if (destination.id == 1 || destination.id == 14)
                return true;
            break;
        case 3:
            if (destination.id == 4 || destination.id == 10)
                return true;
            break;
        case 4:
            if (destination.id == 1 || destination.id == 3 || destination.id == 5 || destination.id == 7)
                return true;
            break;
        case 5:
            if (destination.id == 4 || destination.id == 13)
                return true;
            break;
        case 6:
            if (destination.id == 7 || destination.id == 11)
                return true;
            break;
        case 7:
            if (destination.id == 4 || destination.id == 6 || destination.id == 8)
                return true;
            break;
        case 8:
            if (destination.id == 7 || destination.id == 12)
                return true;
            break;
        case 9:
            if (destination.id == 0 || destination.id == 10 || destination.id == 21)
                return true;
            break;
        case 10:
            if (destination.id == 3 || destination.id == 9 || destination.id == 11 || destination.id == 18)
                return true;
            break;
        case 11:
            if (destination.id == 6 || destination.id == 10 || destination.id == 15)
                return true;
            break;
        case 12:
            if (destination.id == 8 || destination.id == 13 || destination.id == 17)
                return true;
            break;
        case 13:
            if (destination.id == 5 || destination.id == 12 || destination.id == 14 || destination.id == 20)
                return true;
            break;
        case 14:
            if (destination.id == 2 || destination.id == 13 || destination.id == 23)
                return true;
            break;
        case 15:
            if (destination.id == 11 || destination.id == 16)
                return true;
            break;
        case 16:
            if (destination.id == 15 || destination.id == 17 || destination.id == 19)
                return true;
            break;
        case 17:
            if (destination.id == 12 || destination.id == 16)
                return true;
            break;
        case 18:
            if (destination.id == 10 || destination.id == 19)
                return true;
            break;
        case 19:
            if (destination.id == 16 || destination.id == 18 || destination.id == 20 || destination.id == 22)
                return true;
            break;
        case 20:
            if (destination.id == 13 || destination.id == 19)
                return true;
            break;
        case 21:
            if (destination.id == 9 || destination.id == 22)
                return true;
            break;
        case 22:
            if (destination.id == 19 || destination.id == 21 || destination.id == 23)
                return true;
            break;
        case 23:
            if (destination.id == 14 || destination.id == 22)
                return true;
            break;
    }

    return false;
}

static void check_player_pieces_number(entt::registry& registry, entt::entity board, Player player) {
    auto& state = STATE(board);

    if (state.phase == Phase::MovePieces) {
        if (player == Player::White) {
            SPDLOG_DEBUG("White player checked");

            if (state.white_pieces_count == 3) {
                state.can_jump[(int) player] = true;
            } else if (state.white_pieces_count == 2) {
                game_over(registry, board, Ending::WinnerBlack, Piece::White);
            }
        } else {
            SPDLOG_DEBUG("Black player checked");

            if (state.black_pieces_count == 3) {
                state.can_jump[(int) player] = true;
            } else if (state.black_pieces_count == 2) {
                game_over(registry, board, Ending::WinnerWhite, Piece::Black);
            }
        }
    }
}

static bool check_player_blocked(entt::registry& registry, entt::entity board, Player player) {
    auto& state = STATE(board);

    auto view = registry.view<PieceComponent>();

    SPDLOG_DEBUG("{} player is checked if is blocked",
        player == Player::White ? "White" : "Black");

    bool at_least_one_piece = false;
    Piece type = player == Player::White ? Piece::White : Piece::Black;

    if (state.can_jump[(int) player]) {
        return false;
    }

    for (entt::entity entity : view) {
        auto& piece = view.get<PieceComponent>(entity);

        if (piece.type == type && !piece.pending_remove && piece.active) {
            at_least_one_piece = true;

            auto& node = NODE(piece.node);

            switch (node.id) {
                case 0: {
                    auto& node1 = NODE(state.nodes[1]);
                    auto& node2 = NODE(state.nodes[9]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 1: {
                    auto& node1 = NODE(state.nodes[0]);
                    auto& node2 = NODE(state.nodes[2]);
                    auto& node3 = NODE(state.nodes[4]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 2: {
                    auto& node1 = NODE(state.nodes[1]);
                    auto& node2 = NODE(state.nodes[14]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 3: {
                    auto& node1 = NODE(state.nodes[4]);
                    auto& node2 = NODE(state.nodes[10]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 4: {
                    auto& node1 = NODE(state.nodes[1]);
                    auto& node2 = NODE(state.nodes[3]);
                    auto& node3 = NODE(state.nodes[5]);
                    auto& node4 = NODE(state.nodes[7]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null || node4.piece == entt::null)
                        return false;
                    break;
                }
                case 5: {
                    auto& node1 = NODE(state.nodes[4]);
                    auto& node2 = NODE(state.nodes[13]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 6: {
                    auto& node1 = NODE(state.nodes[7]);
                    auto& node2 = NODE(state.nodes[11]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 7: {
                    auto& node1 = NODE(state.nodes[4]);
                    auto& node2 = NODE(state.nodes[6]);
                    auto& node3 = NODE(state.nodes[8]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 8: {
                    auto& node1 = NODE(state.nodes[7]);
                    auto& node2 = NODE(state.nodes[12]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 9: {
                    auto& node1 = NODE(state.nodes[0]);
                    auto& node2 = NODE(state.nodes[10]);
                    auto& node3 = NODE(state.nodes[21]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 10: {
                    auto& node1 = NODE(state.nodes[3]);
                    auto& node2 = NODE(state.nodes[9]);
                    auto& node3 = NODE(state.nodes[11]);
                    auto& node4 = NODE(state.nodes[18]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null || node4.piece == entt::null)
                        return false;
                    break;
                }
                case 11: {
                    auto& node1 = NODE(state.nodes[6]);
                    auto& node2 = NODE(state.nodes[10]);
                    auto& node3 = NODE(state.nodes[15]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 12: {
                    auto& node1 = NODE(state.nodes[8]);
                    auto& node2 = NODE(state.nodes[13]);
                    auto& node3 = NODE(state.nodes[17]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 13: {
                    auto& node1 = NODE(state.nodes[5]);
                    auto& node2 = NODE(state.nodes[12]);
                    auto& node3 = NODE(state.nodes[14]);
                    auto& node4 = NODE(state.nodes[20]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null || node4.piece == entt::null)
                        return false;
                    break;
                }
                case 14: {
                    auto& node1 = NODE(state.nodes[2]);
                    auto& node2 = NODE(state.nodes[13]);
                    auto& node3 = NODE(state.nodes[23]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 15: {
                    auto& node1 = NODE(state.nodes[11]);
                    auto& node2 = NODE(state.nodes[16]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 16: {
                    auto& node1 = NODE(state.nodes[15]);
                    auto& node2 = NODE(state.nodes[17]);
                    auto& node3 = NODE(state.nodes[19]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 17: {
                    auto& node1 = NODE(state.nodes[12]);
                    auto& node2 = NODE(state.nodes[16]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 18: {
                    auto& node1 = NODE(state.nodes[10]);
                    auto& node2 = NODE(state.nodes[19]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 19: {
                    auto& node1 = NODE(state.nodes[16]);
                    auto& node2 = NODE(state.nodes[18]);
                    auto& node3 = NODE(state.nodes[20]);
                    auto& node4 = NODE(state.nodes[22]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null || node4.piece == entt::null)
                        return false;
                    break;
                }
                case 20: {
                    auto& node1 = NODE(state.nodes[13]);
                    auto& node2 = NODE(state.nodes[19]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 21: {
                    auto& node1 = NODE(state.nodes[9]);
                    auto& node2 = NODE(state.nodes[22]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
                case 22: {
                    auto& node1 = NODE(state.nodes[19]);
                    auto& node2 = NODE(state.nodes[21]);
                    auto& node3 = NODE(state.nodes[23]);
                    if (node1.piece == entt::null || node2.piece == entt::null ||
                            node3.piece == entt::null)
                        return false;
                    break;
                }
                case 23: {
                    auto& node1 = NODE(state.nodes[14]);
                    auto& node2 = NODE(state.nodes[22]);
                    if (node1.piece == entt::null || node2.piece == entt::null)
                        return false;
                    break;
                }
            }
        }
    }

    if (at_least_one_piece)
        return true;
    else
        return false;
}

static std::array<Piece, 24> get_position(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    std::array<Piece, 24> position;

    for (int i = 0; i < 24; i++) {
        auto& node = NODE(state.nodes[i]);

        if (node.piece != entt::null) {
            auto& piece = PIECE(node.piece);
            position[i] = piece.type;
        } else {
            position[i] = Piece::None;
        }
    }

    return position;
}

static void remember_position_and_check_repetition(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    std::array<Piece, 24> current_position = get_position(registry, board);

    for (const std::array<Piece, 24>& position : state.repetition_history.twos) {
        if (position == current_position) {
            game_over(registry, board, Ending::TieBetweenBothPlayers,
                state.turn == Player::White ? Piece::White : Piece::Black);
            return;
        }
    }

    for (const std::array<Piece, 24>& position : state.repetition_history.ones) {
        if (position == current_position) {
            std::vector<std::array<Piece, 24>>& vec = state.repetition_history.ones;
            vec.erase(std::remove(vec.begin(), vec.end(), position), vec.end());
            state.repetition_history.twos.push_back(position);
            return;
        }
    }

    state.repetition_history.ones.push_back(current_position);
}

static void clear_repetition_history(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    state.repetition_history.ones.clear();
    state.repetition_history.twos.clear();
}

static void undo_repetition_history(entt::registry& registry, entt::entity board,
                                    const std::array<Piece, 24>& position_to_remove) {
    auto& state = STATE(board);

    for (const std::array<Piece, 24>& position : state.repetition_history.twos) {
        if (position == position_to_remove) {
            std::vector<std::array<Piece, 24>>& vec = state.repetition_history.twos;
            vec.erase(std::remove(vec.begin(), vec.end(), position), vec.end());
            return;
        }
    }

    for (const std::array<Piece, 24>& position : state.repetition_history.ones) {
        if (position == position_to_remove) {
            std::vector<std::array<Piece, 24>>& vec = state.repetition_history.ones;
            vec.erase(std::remove(vec.begin(), vec.end(), position), vec.end());
            return;
        }
    }

    assert(false);
}

void systems::place_piece(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = STATE(board);

    auto view = registry.view<TransformComponent, NodeComponent>();

    for (entt::entity entity : view) {
        auto [transform, node] = view.get(entity);

        if (entity == hovered && entity == state.pressed_node && node.piece == entt::null) {
            const glm::vec3& position = transform.position;

            if (state.turn == Player::White) {
                node.piece = place_new_piece(registry, Piece::White,
                                             position.x, position.z, entity);
                state.white_pieces_count++;   
            } else {
                node.piece = place_new_piece(registry, Piece::Black,
                                             position.x, position.z, entity);
                state.black_pieces_count++;
            }

            state.not_placed_pieces_count--;

            if (is_windmill_made(registry, board, entity,
                    state.turn == Player::White ? Piece::White : Piece::Black)) {
                SPDLOG_DEBUG("Windmill made");
                state.should_take_piece = true;

                if (state.turn == Player::White) {
                    set_pieces_to_take(registry, Piece::Black, true);
                } else {
                    set_pieces_to_take(registry, Piece::White, true);
                }

                state.turns_without_mills = 0;
            } else {
                switch_turn(registry, board);
            }

            if (state.not_placed_pieces_count == 0 && !state.should_take_piece) {
                state.phase = Phase::MovePieces;
                update_outlines(registry, board);

                if (check_player_blocked(registry, board, state.turn)) {
                    game_over(registry, board, state.turn == Player::White ?
                        Ending::WinnerBlack : Ending::WinnerWhite,
                        state.turn == Player::White ? Piece::White : Piece::Black);
                }

                SPDLOG_INFO("Phase 2");
            }

            undo::remember_place(state.moves_history, entity, node.piece);

            break;
        }
    }
}

void systems::move_pieces(entt::registry& registry, float dt) {
    auto view = registry.view<TransformComponent, MoveComponent, PieceComponent>();

    for (entt::entity entity : view) {
        auto [transform, move, piece] = view.get(entity);

        if (move.should_move) {
            if (move.distance_travelled < glm::length(move.distance_to_travel)) {
                glm::vec3 velocity = move.velocity * dt;
                transform.position += velocity;
                move.distance_travelled += glm::length(velocity);
            } else {
                transform.position = move.target;

                // Reset all
                move.should_move = false;
                move.velocity = glm::vec3(0.0f);
                move.target = glm::vec3(0.0f);
                move.distance_travelled = 0.0f;
                move.distance_to_travel = glm::vec3(0.0f);

                // Remove piece if set to remove
                if (piece.pending_remove) {
                    registry.destroy(entity);
                }
            }
        }
    }
}

void systems::take_piece(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = STATE(board);

    auto view = registry.view<NodeComponent>();

    for (entt::entity entity : view) {
        auto& node = view.get<NodeComponent>(entity);

        if (node.piece != entt::null) {
            auto& piece = PIECE(node.piece);
            if (state.turn == Player::White) {
                if (node.piece == hovered && state.pressed_piece == hovered &&
                        piece.type == Piece::Black) {
                    if (!is_windmill_made(registry, board, entity, Piece::Black) ||
                            number_of_pieces_in_windmills(registry, board, Piece::Black) ==
                            state.black_pieces_count) {
                        undo::remember_take(state.moves_history, entity, node.piece);

                        take_raise_piece(registry, node.piece);
                        node.piece = entt::null;
                        switch_turn(registry, board);
                        update_outlines(registry, board);
                        state.should_take_piece = false;
                        set_pieces_to_take(registry, Piece::Black, false);
                        state.black_pieces_count--;

                        check_player_pieces_number(registry, board, state.turn);

                        if (check_player_blocked(registry, board, state.turn)) {
                            game_over(registry, board, state.turn == Player::White ?
                                Ending::WinnerBlack : Ending::WinnerWhite,
                                state.turn == Player::White ? Piece::White : Piece::Black);
                        }

                        clear_repetition_history(registry, board);
                    } else {
                        SPDLOG_DEBUG("Cannot take piece from windmill");
                    }

                    break;
                }
            } else {
                if (node.piece == hovered && state.pressed_piece == hovered &&
                        piece.type == Piece::White) {
                    if (!is_windmill_made(registry, board, entity, Piece::White) ||
                            number_of_pieces_in_windmills(registry, board, Piece::White) ==
                            state.white_pieces_count) {
                        undo::remember_take(state.moves_history, entity, node.piece);

                        take_raise_piece(registry, node.piece);
                        node.piece = entt::null;
                        switch_turn(registry, board);
                        update_outlines(registry, board);
                        state.should_take_piece = false;
                        set_pieces_to_take(registry, Piece::White, false);
                        state.white_pieces_count--;

                        check_player_pieces_number(registry, board, state.turn);

                        if (check_player_blocked(registry, board, state.turn)) {
                            game_over(registry, board, state.turn == Player::White ?
                                Ending::WinnerBlack : Ending::WinnerWhite,
                                state.turn == Player::White ? Piece::White : Piece::Black);
                        }

                        clear_repetition_history(registry, board);
                    } else {
                        SPDLOG_DEBUG("Cannot take piece from windmill");
                    }

                    break;
                }
            }
        }
    }

    // Do this even if it may be not needed
    if (state.phase == Phase::PlacePieces && state.not_placed_pieces_count == 0 &&
            !state.should_take_piece) {
        state.phase = Phase::MovePieces;
        update_outlines(registry, board);
        SPDLOG_INFO("Phase 2");
    }
}

void systems::select_piece(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = STATE(board);

    auto view = registry.view<PieceComponent>();

    for (entt::entity entity : view) {
        auto& piece = view.get<PieceComponent>(entity);

        if (entity == hovered) {
            if (state.turn == Player::White && piece.type == Piece::White ||
                    state.turn == Player::Black && piece.type == Piece::Black) {
                if (!piece.selected && !piece.pending_remove) {
                    state.selected_piece = entity;
                    piece.selected = true;
                    unselect_other_pieces(registry, entity);
                } else {
                    state.selected_piece = entt::null;
                    piece.selected = false;
                }
            }
        }
    }
}

void systems::put_piece(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = STATE(board);

    auto view = registry.view<TransformComponent, NodeComponent>();

    if (state.selected_piece != entt::null) {
        for (entt::entity entity : view) {
            auto [transform, node] = view.get(entity);

            auto& selected_piece = PIECE(state.selected_piece);

            if (entity == hovered && can_go(registry, board, selected_piece.node, entity)) {
                auto& piece_move = registry.get<MoveComponent>(state.selected_piece);
                auto& piece_transform = registry.get<TransformComponent>(state.selected_piece);

                piece_move.target.x = transform.position.x;
                piece_move.target.y = PIECE_Y_POSITION;
                piece_move.target.z = transform.position.z;

                piece_move.velocity = (piece_move.target - piece_transform.position) * PIECE_MOVE_SPEED;
                piece_move.distance_to_travel = piece_move.target - piece_transform.position;
                piece_move.should_move = true;

                undo::remember_move(state.moves_history, selected_piece.node, entity, state.selected_piece);

                // Reset all variables
                auto& previous_node = NODE(selected_piece.node);

                previous_node.piece = entt::null;
                selected_piece.node = entity;
                selected_piece.selected = false;
                node.piece = state.selected_piece;
                state.selected_piece = entt::null;

                if (is_windmill_made(registry, board, entity,
                        state.turn == Player::White ? Piece::White : Piece::Black)) {
                    SPDLOG_DEBUG("Windmill made");
                    state.should_take_piece = true;

                    if (state.turn == Player::White) {
                        set_pieces_to_take(registry, Piece::Black, true);
                        set_pieces_show_outline(registry, Piece::White, false);
                    } else {
                        set_pieces_to_take(registry, Piece::White, true);
                        set_pieces_show_outline(registry, Piece::Black, false);
                    }

                    state.turns_without_mills = 0;
                } else {
                    switch_turn(registry, board);
                    update_outlines(registry, board);

                    if (check_player_blocked(registry, board, state.turn)) {
                        game_over(registry, board, state.turn == Player::White ?
                            Ending::WinnerBlack : Ending::WinnerWhite,
                            state.turn == Player::White ? Piece::White : Piece::Black);
                    }

                    remember_position_and_check_repetition(registry, board);
                }

                break;
            }
        }        
    }
}

void systems::press(entt::registry& registry, entt::entity board, entt::entity hovered) {
    auto& state = STATE(board);

    {
        auto view = registry.view<NodeComponent>();
        for (entt::entity entity : view) {
            if (entity == hovered) {
                state.pressed_node = entity;
            }
        }
    }
    {
        auto view = registry.view<PieceComponent>();
        for (entt::entity entity : view) {
            if (entity == hovered) {
                state.pressed_piece = entity;
            }
        }
    }
}

void systems::release(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    state.pressed_node = entt::null;
    state.pressed_piece = entt::null;
}

void systems::undo_move(entt::registry& registry, entt::entity board) {
    auto& state = STATE(board);

    undo::MoveType move_type = undo::get_undo_type(state.moves_history);

    switch (move_type) {
        case undo::MoveType::Place: {
            undo::PlacedPiece placed_piece = undo::undo_place(state.moves_history);

            auto& node = NODE(placed_piece.node);
            auto& piece = PIECE(placed_piece.piece);

            node.piece = entt::null;
            piece.node = entt::null;
            piece.active = false;

            auto [transform, move] = registry.get<TransformComponent, MoveComponent>(placed_piece.piece);
            move.target = AFTER_UNDO_POSITION;
            move.velocity = (move.target - transform.position) * PIECE_MOVE_SPEED;
            move.distance_to_travel = move.target - transform.position;
            move.should_move = true;

            if (piece.type == Piece::White) {
                state.white_pieces_count--;
            } else {
                state.black_pieces_count--;
            }

            state.not_placed_pieces_count++;
            state.phase = Phase::PlacePieces;

            switch_turn_no_check(registry, board);
            set_pieces_show_outline(registry, Piece::White, false);

            if (state.selected_piece != entt::null) {
                auto& selected_piece = PIECE(state.selected_piece);
                selected_piece.selected = false;
                state.selected_piece = entt::null;
            }

            break;
        }
        case undo::MoveType::Move: {
            undo::MovedPiece moved_piece = undo::undo_move(state.moves_history);

            auto& destination_node = NODE(moved_piece.destination_node);
            auto& source_node = NODE(moved_piece.source_node);
            auto& piece = PIECE(moved_piece.piece);

            undo_repetition_history(registry, board, get_position(registry, board));

            destination_node.piece = entt::null;
            source_node.piece = moved_piece.piece;
            piece.node = moved_piece.source_node;

            auto [transform, move] = registry.get<TransformComponent, MoveComponent>(moved_piece.piece);
            auto& source_node_transform = registry.get<TransformComponent>(moved_piece.source_node);
            move.target.x = source_node_transform.position.x;
            move.target.y = PIECE_Y_POSITION;
            move.target.z = source_node_transform.position.z;
            move.velocity = (move.target - transform.position) * PIECE_MOVE_SPEED;
            move.distance_to_travel = move.target - transform.position;
            move.should_move = true;

            switch_turn_no_check(registry, board);
            update_outlines(registry, board);
            state.turns_without_mills--;

            if (state.selected_piece != entt::null) {
                auto& selected_piece = PIECE(state.selected_piece);
                selected_piece.selected = false;
                state.selected_piece = entt::null;
            }

            break;
        }
        case undo::MoveType::Take: {
            break;
        }
    }
}
