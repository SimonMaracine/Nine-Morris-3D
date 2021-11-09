#include <memory>
#include <array>
#include <vector>
#include <algorithm>

#include "nine_morris_3d/board.h"
#include "nine_morris_3d/hoverable.h"
#include "other/logging.h"

constexpr unsigned int WINDMILLS[16][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

Board::Board(hoverable::Id id) : id(id) {

}

void Board::press(hoverable::Id hovered_id) {
    for (Node& node : nodes) {
        if (node.id == hovered_id) {
            hovered_node = &node;
        }
    }

    for (unsigned int i = 0; i < 18; i++) {
        Piece* piece = pieces[i].get();
        if (piece->id == hovered_id) {
            hovered_piece = piece;
        }
    }
}

void Board::release(hoverable::Id hovered_id) {
    hovered_node = nullptr;
    hovered_piece = nullptr;
}

void Board::place_piece(hoverable::Id hovered_id) {
    for (Node& node : nodes) {
        if (node.id == hovered_id && (&node) == hovered_node && node.piece == nullptr) {
            // TODO remember place

            const glm::vec3& position = node.position;

            if (turn == Player::White) {
                node.piece = place_new_piece(Piece::Type::White, position.x, position.z, &node);
                white_pieces_count++;
            } else {
                node.piece = place_new_piece(Piece::Type::Black, position.x, position.z, &node);
                black_pieces_count++;
            }

            not_placed_pieces_count--;

            // if (is_windmill_made())
        }
    }
}

std::shared_ptr<Piece> Board::place_new_piece(Piece::Type type, float x_pos, float z_pos, Node* node) {
    for (unsigned int i = 0; i < 18; i++) {
        Piece* piece = pieces[i].get();

        if (!piece->in_use && piece->type == type) {
            piece->target.x = x_pos;
            piece->target.y = PIECE_Y_POSITION;
            piece->target.x = z_pos;

            piece->velocity = (piece->target - piece->position) * PIECE_MOVE_SPEED;
            piece->distance_to_travel = piece->target - piece->position;
            piece->should_move = true;

            piece->in_use = true;
            piece->node = node;

            return pieces[i];
        }
    }

    assert(false);
    return nullptr;
}

void Board::take_raise_piece(Piece* piece) {
    piece->target.x = piece->position.x;
    piece->target.y = PIECE_Y_POSITION + 1.5f;
    piece->target.z = piece->position.z;

    piece->velocity = (piece->target - piece->position) * PIECE_MOVE_SPEED;
    piece->distance_to_travel = piece->target - piece->position;
    piece->should_move = true;

    piece->node = nullptr;
    piece->pending_remove = true;
}

void Board::set_pieces_show_outline(Piece::Type type, bool show) {
    for (unsigned int i = 0; i < 18; i++) {
        Piece* piece = pieces[i].get();

        if (piece->type == type) {
            piece->show_outline = show;
        }
    }
}

// TODO remove pieces parameter
void Board::game_over(Board::Ending ending, Piece::Type type_to_hide) {
    phase = Phase::GameOver;
    this->ending = ending;
    set_pieces_show_outline(type_to_hide, false);

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

void Board::switch_turn() {
    if (phase == Phase::MovePieces) {
        turns_without_mills++;

        if (turns_without_mills == MAX_TURNS_WITHOUT_MILLS) {
            game_over(Ending::TieBetweenBothPlayers,
                    turn == Player::White ? Piece::Type::White : Piece::Type::Black);
        }

        if (turn == Player::White) {
            turn = Player::Black;
        } else {
            turn = Player::White;
        }
    }
}

bool Board::is_windmill_made(Node* node, Piece::Type type) {
    for (unsigned int i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        Node& node1 = nodes[mill[0]];
        Node& node2 = nodes[mill[1]];
        Node& node3 = nodes[mill[2]];

        if (node1.piece != nullptr && node2.piece != nullptr && node3.piece != nullptr) {
            Piece* piece1 = node1.piece.get();
            Piece* piece2 = node2.piece.get();
            Piece* piece3 = node3.piece.get();

            if (piece1->type == type && piece2->type == type && piece3->type == type) {
                if (piece1->node == node || piece2->node == node || piece3->node == node) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Board::set_pieces_to_take(Piece::Type type, bool take) {
    for (unsigned int i = 0; i < 18; i++) {
        Piece* piece = pieces[i].get();

        if (piece->type == type) {
            piece->to_take = take;
        }
    }
}

unsigned int Board::number_of_pieces_in_windmills(Piece::Type type) {
    std::vector<std::shared_ptr<Piece>> pieces_inside_mills;

    for (unsigned int i = 0; i < 16; i++) {
        const unsigned int* mill = WINDMILLS[i];

        Node& node1 = nodes[mill[0]];
        Node& node2 = nodes[mill[1]];
        Node& node3 = nodes[mill[2]];

        if (node1.piece != nullptr && node2.piece != nullptr && node3.piece != nullptr) {
            Piece* piece1 = node1.piece.get();
            Piece* piece2 = node2.piece.get();
            Piece* piece3 = node3.piece.get();

            if (piece1->type == type && piece2->type == type && piece3->type == type) {
                std::vector<std::shared_ptr<Piece>>::iterator it;

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

    SPDLOG_DEBUG("Number of {} pieces in mills: {}", type == Piece::Type::White ? "white" : "black",
            pieces_inside_mills.size());

    return pieces_inside_mills.size();
}

float Board::scale = 1.0f;
int Board::index_count = 0;

glm::vec3 Board::specular_color = glm::vec3(0.0f);
float Board::shininess = 0.0f;

int BoardPaint::index_count = 0;
