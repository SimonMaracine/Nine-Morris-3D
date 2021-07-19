#include "application/application.h"

int main() {
    constexpr int width = 1024;
    constexpr int height = 576;

    Application* application = new Application(width, height);
    application->run();
    delete application;
}
