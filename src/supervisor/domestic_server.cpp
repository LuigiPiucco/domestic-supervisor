#include "domestic_server.hpp"

domestic_server::domestic_server(int argc, char **argv)
    : Wt::WServer(argc, argv), mqtt{supervisor_client_id,
                                    {cam_topic, ack_topic}}
{
    using namespace std::literals;

    constexpr auto wait_time = 25ms;
    while (!mqtt.is_connected()) {
        std::this_thread::sleep_for(wait_time);
    }

    auto fn = [this](Wt::WEnvironment const &env) {
        return std::make_unique<domestic_supervisor>(env, mqtt, deepnet);
    };

    addEntryPoint(Wt::EntryPointType::Application, fn, "/");
    addEntryPoint(Wt::EntryPointType::Application, fn, "/blueprint");
    addEntryPoint(Wt::EntryPointType::Application, fn, "/camera");
}