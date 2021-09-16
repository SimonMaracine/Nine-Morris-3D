#include "nine_morris_3d/scenes/loading/loading_scene.h"
#include "nine_morris_3d/options.h"

void LoadingScene::on_enter() {
    options::load_options_from_file(options);
}

void LoadingScene::on_exit() {

}
