#include "application/scenes/loading/loading_scene.h"
#include "other/options.h"

void LoadingScene::on_enter() {
    options::load_options_from_file(options);
}

void LoadingScene::on_exit() {

}
