#pragma once

#include "blueprint.hpp"
#include "constants.hpp"
#include "image_loop.hpp"
#include "mqtt.hpp"
#include "state.hpp"
#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WLength.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <utility>

class domestic_simulator : public Wt::WApplication {
public:
    domestic_simulator(Wt::WEnvironment const &env, controllers::mqtt &mqtt);
    domestic_simulator(domestic_simulator const &d) = delete;
    domestic_simulator(domestic_simulator &&d) = delete;
    auto operator=(domestic_simulator const &d)
        -> domestic_simulator & = delete;
    auto operator=(domestic_simulator &&d) -> domestic_simulator & = delete;
    ~domestic_simulator() override;

private:
    controllers::state state;
    controllers::mqtt &mqtt;

    views::blueprint *blueprint_view;

    void
    perform_state_change_mqtt(controllers::mqtt::incoming_event const &ctx);
    void perform_state_change_click(std::size_t const &ctx);
    void update_blueprint_view(std::pair<std::size_t, bool> const &ctx);
    void send_acknowledged_signal(std::pair<std::size_t, bool> const &ctx);
};
