#include "domestic_simulator.hpp"

auto main(int argc, char **argv) -> int
{
    controllers::mqtt mqtt{simulator_client_id, {state_topic}};
    controllers::image_loop image_loop(mqtt);

    return Wt::WRun(argc, argv, [&mqtt](Wt::WEnvironment const &env) {
        return std::make_unique<domestic_simulator>(env, mqtt);
    });
}
