#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "nine_morris_3d.h"
#include "game/components/board.h"
#include "game/components/piece.h"
#include "game/components/node.h"
#include "game/components/active.h"
#include "game/components/piece_move.h"
#include "game/components/undo_redo_state.h"
#include "game/components/game_context.h"
#include "game/components/options.h"
#include "game/systems/board.h"
#include "game/board_common.h"
#include "game/constants.h"

entt::entity new_piece_to_place(entt::registry& registry, PieceType type, float x_pos, float z_pos, entt::entity node) {
    auto view = registry.view<TransformComponent, PieceComponent, ActiveComponent>();

    for (auto entity : view) {
        auto [transform_c, piece_c] = view.get<TransformComponent, PieceComponent>(entity);

        if (!piece_c.in_use && piece_c.type == type) {
            const glm::vec3 target = glm::vec3(x_pos, PIECE_Y_POSITION, z_pos);
            const glm::vec3 target0 = transform_c.position + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 target1 = target + glm::vec3(0.0f, PIECE_THREESTEP_HEIGHT, 0.0f);
            const glm::vec3 velocity = glm::normalize(target0 - transform_c.position) * PIECE_BASE_VELOCITY;

            prepare_piece_for_three_step_move(registry, entity, target, velocity, target0, target1);

            piece_c.in_use = true;
            piece_c.node = node;
            piece_c.node_index = registry.get<NodeComponent>(node).index;

            return entity;
        }
    }

    ASSERT(false, "Couldn't find a piece");
    return entt::null;
}

void take_and_raise_piece(entt::registry& registry, entt::entity piece) {
    auto [transform_c, piece_c] = registry.get<TransformComponent, PieceComponent>(piece);

    prepare_piece_for_linear_move(
        registry,
        piece,
        glm::vec3(transform_c.position.x, PIECE_Y_POSITION + PIECE_RAISE_HEIGHT, transform_c.position.z),
        glm::vec3(0.0f)
    );

    piece_c.node = entt::null;
    piece_c.node_index = INVALID_NODE_INDEX;
    piece_c.pending_remove = true;
}

void set_pieces_show_outline(entt::registry& registry, PieceType type, bool show) {
    auto view = registry.view<PieceComponent, ActiveComponent>();

    for (auto entity : view) {
        auto& piece_c = view.get<PieceComponent>(entity);

        if (piece_c.type == type) {
            piece_c.show_outline = show;
        }
    }
}

void game_over(entt::registry& registry, BoardEnding ending, PieceType type_to_hide, std::string_view ending_message) {
    auto view = registry.view<BoardComponent>();
    auto& board_c = view.get<BoardComponent>(view.back());

    board_c.phase = BoardPhase::GameOver;
    board_c.ending = ending;
    board_c.ending_message = ending_message;
    set_pieces_show_outline(registry, type_to_hide, false);

    switch (ending) {
        case BoardEnding::WinnerWhite:
            DEB_INFO("Game over, white wins");
            break;
        case BoardEnding::WinnerBlack:
            DEB_INFO("Game over, black wins");
            break;
        case BoardEnding::TieBetweenBothPlayers:
            DEB_INFO("Game over, tie between both players");
            break;
        default:
            ASSERT(false, "Invalid ending");
    }
}

void switch_turn(entt::registry& registry) {
    auto view = registry.view<BoardComponent>();
    auto& board_c = view.get<BoardComponent>(view.back());

    if (board_c.phase == BoardPhase::MovePieces) {
        if (board_c.turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            DEB_INFO("The max amount of turns without mills has been hit");

            FORMATTED_MESSAGE(
                message, 64, "%u turns have passed without a windmill made.",
                MAX_TURNS_WITHOUT_MILLS
            )

            game_over(
                registry,
                BoardEnding::TieBetweenBothPlayers,
                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                message
            );
        }
    }

    board_c.turn = TURN_IS_WHITE_SO(BoardPlayer::Black, BoardPlayer::White);
    board_c.switched_turn = true;
}

bool is_windmill_made(entt::registry& registry, entt::entity node, PieceType type) {
    auto view = registry.view<BoardComponent>();
    auto& board_c = view.get<BoardComponent>(view.back());

    for (size_t i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[mill[0]]);
        const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[mill[1]]);
        const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[mill[2]]);

        if (node_c1.piece != entt::null && node_c2.piece != entt::null && node_c3.piece != entt::null) {
            const auto& piece_c1 = registry.get<PieceComponent>(node_c1.piece);
            const auto& piece_c2 = registry.get<PieceComponent>(node_c2.piece);
            const auto& piece_c3 = registry.get<PieceComponent>(node_c3.piece);

            if (piece_c1.type == type && piece_c2.type == type && piece_c3.type == type) {
                if (piece_c1.node == node || piece_c2.node == node || piece_c3.node == node) {
                    return true;
                }
            }
        }
    }

    return false;
}

void set_pieces_to_take(entt::registry& registry, PieceType type, bool take) {
    auto view = registry.view<PieceComponent, ActiveComponent>();

    for (auto entity : view) {
        auto& piece_c = view.get<PieceComponent>(entity);

        if (piece_c.type == type) {
            piece_c.to_take = take;
        }
    }
}

size_t number_of_pieces_in_windmills(entt::registry& registry, PieceType type) {
    auto view = registry.view<BoardComponent>();
    auto& board_c = view.get<BoardComponent>(view.back());

    std::vector<entt::entity> pieces_inside_mills;

    for (size_t i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[mill[0]]);
        const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[mill[1]]);
        const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[mill[2]]);

        if (node_c1.piece != entt::null && node_c2.piece != entt::null && node_c3.piece != entt::null) {
            const auto& piece_c1 = registry.get<PieceComponent>(node_c1.piece);
            const auto& piece_c2 = registry.get<PieceComponent>(node_c2.piece);
            const auto& piece_c3 = registry.get<PieceComponent>(node_c3.piece);

            if (piece_c1.type == type && piece_c2.type == type && piece_c3.type == type) {
                std::vector<entt::entity>::iterator iter;

                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node_c1.piece);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node_c1.piece);
                }
                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node_c2.piece);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node_c2.piece);
                }
                iter = std::find(pieces_inside_mills.begin(), pieces_inside_mills.end(), node_c3.piece);
                if (iter == pieces_inside_mills.end()) {
                    pieces_inside_mills.push_back(node_c3.piece);
                }
            }
        }
    }

    DEB_DEBUG(
        "Number of {} pieces in mills: {}",
        type == PieceType::White ? "white" : "black",
        pieces_inside_mills.size()
    );

    return pieces_inside_mills.size();
}

void unselect_other_pieces(entt::registry& registry, entt::entity currently_selected_piece) {
    auto view = registry.view<PieceComponent, ActiveComponent>();

    for (auto entity : view) {
        if (entity != currently_selected_piece) {
            auto& piece_c = view.get<PieceComponent>(entity);
            piece_c.selected = false;
        }
    } 
}

void update_outlines(entt::registry& registry) {
    auto view = registry.view<BoardComponent>();
    auto& board_c = view.get<BoardComponent>(view.back());

    if (board_c.phase == BoardPhase::MovePieces) {
        if (board_c.turn == BoardPlayer::White) {
            set_pieces_show_outline(registry, PieceType::White, true);
            set_pieces_show_outline(registry, PieceType::Black, false);
        } else {
            set_pieces_show_outline(registry, PieceType::Black, true);
            set_pieces_show_outline(registry, PieceType::White, false);
        }
    }
}

bool can_go(entt::registry& registry, entt::entity source_node, entt::entity destination_node) {
    ASSERT(source_node != destination_node, "Source must be different than destination");

    auto view = registry.view<BoardComponent>();
    auto& board_c = view.get<BoardComponent>(view.back());

    if (board_c.can_jump[static_cast<int>(board_c.turn)]) {
        return true;
    }

    auto& node_c_source = registry.get<NodeComponent>(source_node);
    auto& node_c_destination = registry.get<NodeComponent>(destination_node);

    switch (node_c_source.index) {
        case 0:
            if (node_c_destination.index == 1 || node_c_destination.index == 9)
                return true;
            break;
        case 1:
            if (node_c_destination.index == 0 || node_c_destination.index == 2 || node_c_destination.index == 4)
                return true;
            break;
        case 2:
            if (node_c_destination.index == 1 || node_c_destination.index == 14)
                return true;
            break;
        case 3:
            if (node_c_destination.index == 4 || node_c_destination.index == 10)
                return true;
            break;
        case 4:
            if (node_c_destination.index == 1 || node_c_destination.index == 3 || node_c_destination.index == 5
                    || node_c_destination.index == 7)
                return true;
            break;
        case 5:
            if (node_c_destination.index == 4 || node_c_destination.index == 13)
                return true;
            break;
        case 6:
            if (node_c_destination.index == 7 || node_c_destination.index == 11)
                return true;
            break;
        case 7:
            if (node_c_destination.index == 4 || node_c_destination.index == 6 || node_c_destination.index == 8)
                return true;
            break;
        case 8:
            if (node_c_destination.index == 7 || node_c_destination.index == 12)
                return true;
            break;
        case 9:
            if (node_c_destination.index == 0 || node_c_destination.index == 10 || node_c_destination.index == 21)
                return true;
            break;
        case 10:
            if (node_c_destination.index == 3 || node_c_destination.index == 9 || node_c_destination.index == 11
                    || node_c_destination.index == 18)
                return true;
            break;
        case 11:
            if (node_c_destination.index == 6 || node_c_destination.index == 10 || node_c_destination.index == 15)
                return true;
            break;
        case 12:
            if (node_c_destination.index == 8 || node_c_destination.index == 13 || node_c_destination.index == 17)
                return true;
            break;
        case 13:
            if (node_c_destination.index == 5 || node_c_destination.index == 12 || node_c_destination.index == 14
                    || node_c_destination.index == 20)
                return true;
            break;
        case 14:
            if (node_c_destination.index == 2 || node_c_destination.index == 13 || node_c_destination.index == 23)
                return true;
            break;
        case 15:
            if (node_c_destination.index == 11 || node_c_destination.index == 16)
                return true;
            break;
        case 16:
            if (node_c_destination.index == 15 || node_c_destination.index == 17 || node_c_destination.index == 19)
                return true;
            break;
        case 17:
            if (node_c_destination.index == 12 || node_c_destination.index == 16)
                return true;
            break;
        case 18:
            if (node_c_destination.index == 10 || node_c_destination.index == 19)
                return true;
            break;
        case 19:
            if (node_c_destination.index == 16 || node_c_destination.index == 18 || node_c_destination.index == 20
                    || node_c_destination.index == 22)
                return true;
            break;
        case 20:
            if (node_c_destination.index == 13 || node_c_destination.index == 19)
                return true;
            break;
        case 21:
            if (node_c_destination.index == 9 || node_c_destination.index == 22)
                return true;
            break;
        case 22:
            if (node_c_destination.index == 19 || node_c_destination.index == 21 || node_c_destination.index == 23)
                return true;
            break;
        case 23:
            if (node_c_destination.index == 14 || node_c_destination.index == 22)
                return true;
            break;
    }

    return false;
}

void check_player_number_of_pieces(entt::registry& registry, BoardPlayer player) {
    const auto view = registry.view<BoardComponent>();
    auto& board_c = view.get<BoardComponent>(view.back());

    if (player == BoardPlayer::White) {
        DEB_DEBUG("Checking white player number of pieces");

        if (board_c.white_pieces_count + board_c.not_placed_pieces_count == 3) {  // TODO really think this through
            board_c.can_jump[static_cast<int>(player)] = true;

            DEB_INFO("White player can jump");
        } else if (board_c.white_pieces_count + board_c.not_placed_pieces_count == 2) {
            DEB_INFO("White player has only 2 pieces");

            FORMATTED_MESSAGE(
                message, 64, "White player cannot make any more windmills."
            )

            game_over(registry, BoardEnding::WinnerBlack, PieceType::White, message);
        }
    } else {
        DEB_DEBUG("Checking black player number of pieces");

        if (board_c.black_pieces_count + board_c.not_placed_pieces_count == 3) {
            board_c.can_jump[static_cast<int>(player)] = true;

            DEB_INFO("Black player can jump");
        } else if (board_c.black_pieces_count + board_c.not_placed_pieces_count == 2) {
            DEB_INFO("Black player has only 2 pieces");

            FORMATTED_MESSAGE(
                message, 64, "Black player cannot make any more windmills."
            )

            game_over(registry, BoardEnding::WinnerWhite, PieceType::Black, message);
        }
    }
}

bool is_player_blocked(entt::registry& registry, BoardPlayer player) {
    DEB_DEBUG("{} player is checked if is blocked", player == BoardPlayer::White ? "White" : "Black");

    const auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    const PieceType type = player == BoardPlayer::White ? PieceType::White : PieceType::Black;
    bool at_least_one_piece = false;

    if (board_c.can_jump[static_cast<int>(player)]) {
        return false;
    }

    auto view = registry.view<PieceComponent, ActiveComponent>();

    for (auto entity : view) {
        const auto& piece_c = view.get<PieceComponent>(entity);

        if (piece_c.type == type && !piece_c.pending_remove && piece_c.in_use) {
            at_least_one_piece = true;

            const auto& node_c = registry.get<NodeComponent>(piece_c.node);

            switch (node_c.index) {
                case 0: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[1]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[9]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 1: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[0]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[2]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[4]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 2: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[1]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[14]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 3: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[4]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[10]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 4: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[1]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[3]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[5]);
                    const auto& node_c4 = registry.get<NodeComponent>(board_c.nodes[7]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null
                            || node_c3.piece == entt::null || node_c4.piece == entt::null)
                        return false;
                    break;
                }
                case 5: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[4]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[13]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 6: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[7]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[11]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 7: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[4]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[6]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[8]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 8: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[7]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[12]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 9: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[0]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[10]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[21]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 10: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[3]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[9]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[11]);
                    const auto& node_c4 = registry.get<NodeComponent>(board_c.nodes[18]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null
                            || node_c3.piece == entt::null || node_c4.piece == entt::null)
                        return false;
                    break;
                }
                case 11: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[6]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[10]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[15]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 12: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[8]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[13]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[17]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 13: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[5]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[12]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[14]);
                    const auto& node_c4 = registry.get<NodeComponent>(board_c.nodes[20]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null
                            || node_c3.piece == entt::null || node_c4.piece == entt::null)
                        return false;
                    break;
                }
                case 14: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[2]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[13]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[23]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 15: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[11]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[16]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 16: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[15]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[17]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[19]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 17: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[12]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[16]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 18: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[10]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[19]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 19: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[16]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[18]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[20]);
                    const auto& node_c4 = registry.get<NodeComponent>(board_c.nodes[22]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null
                            || node_c3.piece == entt::null || node_c4.piece == entt::null)
                        return false;
                    break;
                }
                case 20: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[13]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[19]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 21: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[9]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[22]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
                case 22: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[19]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[21]);
                    const auto& node_c3 = registry.get<NodeComponent>(board_c.nodes[23]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null || node_c3.piece == entt::null)
                        return false;
                    break;
                }
                case 23: {
                    const auto& node_c1 = registry.get<NodeComponent>(board_c.nodes[14]);
                    const auto& node_c2 = registry.get<NodeComponent>(board_c.nodes[22]);
                    if (node_c1.piece == entt::null || node_c2.piece == entt::null)
                        return false;
                    break;
                }
            }
        }
    }

    if (at_least_one_piece) {
        return true;
    } else {
        return false;
    }
}

void remember_position_and_check_repetition(entt::registry& registry, entt::entity piece, entt::entity node) {
    using Position = ThreefoldRepetitionHistory::PositionPlusInfo;

    ASSERT(piece != entt::null, "Piece must not be null");
    ASSERT(node != entt::null, "Node must not be null");

    const auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    const auto& piece_c = registry.get<PieceComponent>(piece);
    const auto& node_c = registry.get<NodeComponent>(node);

    const Position current_position = { get_position(registry), piece_c.index, node_c.index };

    for (const Position& position : board_c.repetition_history.twos) {
        if (position == current_position) {
            DEB_INFO("Threefold repetition");

            FORMATTED_MESSAGE(
                message, 64, "%s player has made threefold repetition.",
                TURN_IS_WHITE_SO("Black", "White")
            )

            game_over(
                registry,
                BoardEnding::TieBetweenBothPlayers,
                TURN_IS_WHITE_SO(PieceType::White, PieceType::Black),
                message
            );
            return;
        }
    }

    for (const Position& position : board_c.repetition_history.ones) {
        if (position == current_position) {
            std::vector<Position>& ones = board_c.repetition_history.ones;

            auto iter = std::find(ones.begin(), ones.end(), position);
            ASSERT(iter != ones.end(), "That should be impossible");

            // This invalidates repetition_history.ones, but it's okay, because we return
            ones.erase(iter);

            // Insert current_position, because position is invalidated
            board_c.repetition_history.twos.push_back(current_position);
            return;
        }
    }

    board_c.repetition_history.ones.push_back(current_position);
}

void remember_state(entt::registry& registry) {
    const auto board_view = registry.view<BoardComponent>();
    const auto& board_c = board_view.get<BoardComponent>(board_view.back());

    const auto camera_view = registry.view<CameraComponent>();
    const auto& camera_c = camera_view.get<CameraComponent>(camera_view.back());

    const auto game_context_view = registry.view<GameContextComponent>();
    const auto& game_context_c = game_context_view.get<GameContextComponent>(game_context_view.back());

    const auto undo_redo_state_view = registry.view<UndoRedoStateComponent>();
    auto& undo_redo_state_c = undo_redo_state_view.get<UndoRedoStateComponent>(undo_redo_state_view.back());

    const UndoRedoStateComponent::Page page = {
        board_c, camera_c, game_context_c.state
    };

    undo_redo_state_c.undo.push_back(page);
    undo_redo_state_c.redo.clear();

    DEB_DEBUG("Pushed new state");
}

void arrive_at_node(entt::registry& registry, entt::entity piece) {
    const auto board_view = registry.view<BoardComponent>();
    auto& board_c = board_view.get<BoardComponent>(board_view.back());

    auto [transform_c, piece_c, move_c] = registry.get<TransformComponent, PieceComponent, PieceMoveComponent>(piece);

    transform_c.position = move_c.target;

    // Reset all these movement variables
    registry.replace<PieceMoveComponent>(piece);

    // Remove piece if set to remove
    if (piece_c.pending_remove) {
        piece_c.pending_remove = false;
        registry.erase<ActiveComponent>(piece);
        // app->renderer->remove_model(piece->model.handle);
        registry.erase<ModelComponent>(piece);
    }

    CAN_MAKE_MOVE();
}

void prepare_piece_for_linear_move(entt::registry& registry, entt::entity piece, const glm::vec3& target, const glm::vec3& velocity) {
    auto& move_c = registry.get<PieceMoveComponent>(piece);

    move_c.target = target;

    move_c.velocity = velocity;

    move_c.type = PieceMovementType::Linear;
    move_c.moving = true;
}

void prepare_piece_for_three_step_move(entt::registry& registry, entt::entity piece, const glm::vec3& target,
        const glm::vec3& velocity, const glm::vec3& target0, const glm::vec3& target1) {
    auto& move_c = registry.get<PieceMoveComponent>(piece);

    move_c.target = target;

    move_c.velocity = velocity;
    move_c.target0 = target0;
    move_c.target1 = target1;

    move_c.type = PieceMovementType::ThreeStep;
    move_c.moving = true;
}

GamePosition get_position(entt::registry& registry) {
    GamePosition position;

    auto view = registry.view<NodeComponent>();

    for (auto entity : view) {
        const auto& node_c = view.get<NodeComponent>(entity);

        if (node_c.piece != entt::null) {
            position[node_c.index] = registry.get<PieceComponent>(node_c.piece).type;
        } else {
            position[node_c.index] = PieceType::None;
        }
    }

    return position;
}
