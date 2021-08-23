#include "application/application.h"
#include "application/layers/game_layer.h"
#include "application/layers/loading_layer.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height);
    application->push_layer(new LoadingLayer(0, application));
    application->push_layer(new GameLayer(1, application));
    application->run();
    delete application;
}
