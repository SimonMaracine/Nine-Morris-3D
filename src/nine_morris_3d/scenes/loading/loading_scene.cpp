#include "other/logging.h"
#include "nine_morris_3d/scenes/loading/loading_scene.h"
#include "nine_morris_3d/options.h"

void LoadingScene::on_enter() {
    SPDLOG_DEBUG("Enter loading scene");

    options::load_options_from_file(options);
}

void LoadingScene::on_exit() {
    SPDLOG_DEBUG("Exit loading scene");
}
