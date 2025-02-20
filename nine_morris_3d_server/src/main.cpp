#include <iostream>
#include <csignal>

#include <networking/server.hpp>

#include "server.hpp"
#include "clock.hpp"
#include "daemon.hpp"
#include "platform.hpp"

#ifdef SM_BUILD_DISTRIBUTION
    static const char* CONFIGURATION_FILE_PATH {"/etc/ninemorris3d/nine_morris_3d_config.json"};
#else
    static const char* CONFIGURATION_FILE_PATH {"nine_morris_3d_config.json"};
#endif

static volatile std::sig_atomic_t g_running {1};

extern "C" void handler(int) {
    g_running = 0;
}

int main() {
    if (std::signal(SIGTERM, handler) == SIG_ERR) {
        std::cerr << "Could not set sigterm signal handler\n";
        notify_stopping("Could not set sigterm signal handler");
        return 1;
    }

    if (std::signal(SIGINT, handler) == SIG_ERR) {
        std::cerr << "Could not set sigint signal handler\n";
        notify_stopping("Could not set sigint signal handler");
        return 1;
    }

    Configuration configuration;

    try {
        read_configuration(configuration, CONFIGURATION_FILE_PATH);
    } catch (const ConfigurationError& e) {
        std::cerr << "Could not read configuration file: " << e.what() << '\n';

        try {
            write_configuration(configuration, CONFIGURATION_FILE_PATH);
        } catch (const ConfigurationError& e) {
            std::cerr << "Could not write configuration file: " << e.what() << '\n';
        }
    }

    using namespace std::chrono_literals;

    try {
        Server server {configuration};
        server.start(configuration);

        notify_ready();

        while (g_running) {
            // Start the clock
            Clock clock;

            server.update();

            // 60 UPS is fine for now
            clock.sleep(16ms);
        }
    } catch (const networking::ConnectionError&) {
        std::cerr << "A fatal error occurred\n";
        notify_stopping("A fatal error occurred");
        return 1;
    } catch (const networking::ServerError& e) {
        std::cerr << "A fatal error occurred: " << e.what() << '\n';
        notify_stopping("A fatal error occurred: %s", e.what());
        return 1;
    }

    notify_stopping();

    return 0;
}
