#include "domestic_server.hpp"
#include <spdlog/spdlog.h>

auto main(int argc, char **argv) -> int
{
    try {
        domestic_server server(argc, argv);
        if (server.start()) {
            int sig = domestic_server::waitForShutdown();
            spdlog::error("Shutdown signal: {}.", sig);
            server.stop();

            return 0;
        }
    }
    catch (domestic_server::Exception &e) {
        spdlog::error("{}.", e.what());
        return 1;
    }
    catch (std::exception &e) {
        spdlog::error("Exception: {}.", e.what());
        return 1;
    }

    return 0;
}