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

    Configuration configuration;

    try {
        load_configuration(configuration, "nine_morris_3d_config.json");
    } catch (const ConfigurationError& e) {
        std::cerr << "Could not load configuration file: " << e.what() << '\n';

        try {
            save_configuration(configuration, "nine_morris_3d_config.json");
        } catch (const ConfigurationError& e) {
            std::cerr << "Could not save configuration file: " << e.what() << '\n';
        }
    }

    try {
        Server server {configuration};
        server.start(configuration);

        while (g_running) {
            server.update();
        }
    } catch (const networking::ConnectionError&) {
        std::cerr << "A fatal error occurred\n";
        return 1;
    } catch (const networking::ServerError& e) {
        std::cerr << "A fatal error occurred: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
