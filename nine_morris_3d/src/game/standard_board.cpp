#include "game/standard_board.hpp"

#include <utility>

StandardBoard::StandardBoard(sm::Renderable&& renderable)
    : renderable(std::move(renderable)) {
    this->renderable.transform.scale = 20.0f;
}

void StandardBoard::update(sm::Ctx& ctx) {
    ctx.add_renderable(renderable);
}
