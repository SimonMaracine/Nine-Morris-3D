#include "application/application.h"
#include "application/layers/game_layer.h"
#include "application/layers/loading_layer.h"
#include "application/layers/gui_layer.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height);
    application->push_layer(new GameLayer(2, application));
    application->push_layer(new GuiLayer(1, application));
    application->push_layer(new LoadingLayer(0, application));
    application->run();
    delete application;
}
