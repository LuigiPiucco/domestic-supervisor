#pragma once

#include "blueprint.hpp"
#include "camera.hpp"
#include "deepnet.hpp"
#include "mqtt.hpp"
#include "page_anchor.hpp"
#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>
#include <string>
#include <vector>

class domestic_supervisor : public Wt::WApplication {
public:
    domestic_supervisor(Wt::WEnvironment const &env, controllers::mqtt &mqtt,
                        controllers::deepnet &deepnet);
    domestic_supervisor(domestic_supervisor const &d) = delete;
    domestic_supervisor(domestic_supervisor &&d) = delete;
    auto operator=(domestic_supervisor const &d)
        -> domestic_supervisor & = delete;
    auto operator=(domestic_supervisor &&d) -> domestic_supervisor & = delete;
    ~domestic_supervisor() override;

private:
    controllers::mqtt &mqtt;
    controllers::deepnet &deepnet;
    Wt::WStackedWidget *container;
    views::camera *camera_page;
    views::blueprint *blueprint_page;

    void perform_path_change(std::string const &ctx);
    void
    perform_state_change_mqtt(controllers::mqtt::incoming_event const &ctx);
    void send_state_change_mqtt(std::size_t const &ctx);
    void perform_image_swap(controllers::deepnet::image const &ctx);
    void receive_image(controllers::mqtt::incoming_event const &ctx);
    void perform_deepnet_state_change();
    void perform_deepnet_num_change(int const &ctx);
    void perform_msg_setup();
};