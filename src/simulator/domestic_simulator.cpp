#include "domestic_simulator.hpp"

domestic_simulator::domestic_simulator(Wt::WEnvironment const &env,
                                       controllers::mqtt &mqtt)
    : Wt::WApplication(env), state{devices[0],  devices[1],  devices[2],
                                   devices[3],  devices[4],  devices[5],
                                   devices[6],  devices[7],  devices[8],
                                   devices[9],  devices[10], devices[11],
                                   devices[12], devices[13], devices[14],
                                   devices[15], devices[16], devices[17]},
      mqtt(mqtt)
{
    setTitle("Domestic Simulator");

    enableUpdates();

    styleSheet().addRule("body", "width: 100vw; height: 100vh; overflow: "
                                 "hidden; margin: 0; background-color: black;");

    blueprint_view = root()->addNew<views::blueprint>(
        std::vector<device>{devices.begin(), devices.end()});

    mqtt.incoming().connect(&domestic_simulator::perform_state_change_mqtt,
                            this);
    blueprint_view->device_clicked().connect(
        &domestic_simulator::perform_state_change_click, this);
    state.changed().connect(&domestic_simulator::update_blueprint_view, this);
    state.changed().connect(&domestic_simulator::send_acknowledged_signal,
                            this);
}

domestic_simulator::~domestic_simulator() { mqtt.incoming().disconnect(this); }

void domestic_simulator::perform_state_change_mqtt(
    controllers::mqtt::incoming_event const &ctx)
{
    if (ctx.topic() == state_topic) {
        try {
            int dev_number = std::atoi(ctx.message().data());
            if (dev_number < 0 || dev_number >= device_names.size()) {
                throw std::out_of_range(std::to_string(dev_number));
            }

            state.toggle(dev_number);
        }
        catch (std::invalid_argument &e) {
            spdlog::error("Invalid message \"{}\" given. Ignoring.",
                          ctx.message());
        }
        catch (std::out_of_range &e) {
            spdlog::error(
                "Processed payload \"{}\" is out-of-bounds in the device "
                "list. Ignoring.",
                e.what());
        }
    }
}

void domestic_simulator::perform_state_change_click(std::size_t const &ctx)
{
    state.toggle(ctx);
}

void domestic_simulator::update_blueprint_view(
    std::pair<std::size_t, bool> const &ctx)
{
    auto const &[dev, _] = ctx;

    if (UpdateLock l{this}) {
        blueprint_view->redraw_state(dev);
        triggerUpdate();
    }
}

void domestic_simulator::send_acknowledged_signal(
    std::pair<std::size_t, bool> const &ctx)
{
    auto const &[dev, _] = ctx;

    mqtt.publish_message(std::string(ack_topic), std::to_string(dev), 2);
}
