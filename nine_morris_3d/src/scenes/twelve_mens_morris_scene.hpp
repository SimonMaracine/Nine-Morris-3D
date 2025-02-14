#pragma once

#include "nine_mens_morris_base_scene.hpp"

struct TwelveMensMorrisScene : NineMensMorrisBaseScene {
    explicit TwelveMensMorrisScene(sm::Ctx& ctx)
        : NineMensMorrisBaseScene(ctx) {}

    SM_SCENE_NAME("twelve_mens_morris")

    std::filesystem::path saved_games_file_path() const override { return ctx.path_saved_data("twelve_mens_morris.dat"); }
    bool twelve_mens_morris() const override { return true; }
};
