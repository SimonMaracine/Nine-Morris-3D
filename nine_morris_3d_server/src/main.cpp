#include <iostream>
#include <csignal>

#include <networking/server.hpp>

#include "server.hpp"
#include "clock.hpp"
#include "daemon.hpp"
#include "platform.hpp"

#ifdef SM_BUILD_DISTRIBUTION
    static constexpr const char* CONFIGURATION_DIRECTORY_PATH {"/etc/ninemorris3d"};
    static constexpr const char* CONFIGURATION_FILE_PATH {"/etc/ninemorris3d/nine_morris_3d.json"};
    static constexpr const char* LOG_FILE_PATH {"/var/log/ninemorris3d/nine_morris_3d.log"};
    static constexpr auto SIGNAL_TYPE {SIGTERM};
#else
    static constexpr const char* CONFIGURATION_FILE_PATH {"nine_morris_3d.json"};
    static constexpr const char* LOG_FILE_PATH {"nine_morris_3d.log"};
    static constexpr auto SIGNAL_TYPE {SIGINT};
#endif

static volatile std::sig_atomic_t g_running {1};

extern "C" void handler(int) {
    g_running = 0;
}

int main() {
    if (std::signal(SIGNAL_TYPE, handler) == SIG_ERR) {
        std::cerr << "Could not set signal handler\n";
        notify_stopping("Could not set signal handler");
        return 1;
    }

#ifdef SM_BUILD_DISTRIBUTION
    try {
        make_configuration_directory(CONFIGURATION_DIRECTORY_PATH);
    } catch (const ConfigurationError& e) {
        std::cerr << "Error configuration directory: " << e.what() << '\n';
    }
#endif

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
        Server server {configuration, LOG_FILE_PATH};
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
