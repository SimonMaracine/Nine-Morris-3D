#include <iostream>
#include <csignal>

#include <networking/server.hpp>

#include "server.hpp"

static volatile std::sig_atomic_t g_running {1};

extern "C" void handler(int) {
    g_running = 0;
}

int main() {
    if (std::signal(SIGINT, handler) == SIG_ERR) {
        std::cerr << "Could not set signal handler\n";
        return 1;
    }

    Server server;

    try {
        server.start();

        while (g_running) {
            server.update();
        }
    } catch (const networking::ConnectionError&) {
        std::cerr << "A fatal error occurred\n";
        return 1;
    }

    return 0;
}
