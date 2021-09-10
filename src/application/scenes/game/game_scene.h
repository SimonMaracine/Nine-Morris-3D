#pragma once

#include "application/scene.h"
#include "other/options.h"

class GameScene : public Scene {
public:
    GameScene(unsigned int id)
        : Scene(id) {}
    virtual ~GameScene() = default;

    virtual void on_enter() override;
    virtual void on_exit() override;

    options::Options options;

    entt::entity board = entt::null;
    entt::entity camera = entt::null;
    entt::entity nodes[24];
    entt::entity pieces[18];

    entt::entity hovered_entity = entt::null;
};
