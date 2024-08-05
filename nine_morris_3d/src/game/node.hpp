#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Node {
public:
    Node() = default;
    Node(unsigned int index, glm::vec3 position, sm::Renderable&& renderable);

    void update(sm::Ctx& ctx);
private:
    bool highlighted {false};
    unsigned int index {};

    sm::Renderable renderable;
};
