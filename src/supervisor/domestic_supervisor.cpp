#include "domestic_supervisor.hpp"

domestic_supervisor::domestic_supervisor(Wt::WEnvironment const &env,
                                         controllers::mqtt &mqtt,
                                         controllers::deepnet &deepnet)
    : Wt::WApplication(env), mqtt(mqtt), deepnet(deepnet)
{

    enableInternalPaths();
    enableUpdates();

    setTitle("Domestic Supervisor");

    styleSheet().addRule(
        "*", "font-size: 18px; font-family: Fira Code, monospace; "
             "font-style: normal; font-weight: normal; line-height: 24px;");
    styleSheet().addRule("body", "width: 100vw; height: 100vh; overflow: "
                                 "hidden; margin: 0; background-color: black;");

    container = root()->addNew<Wt::WStackedWidget>();
    camera_page = container->addNew<views::camera>();
    blueprint_page = container->addNew<views::blueprint>(
        std::vector<device>{devices.begin(), devices.end()});
    constexpr auto top = 32;
    constexpr auto left = 833;
    auto *to_blueprint_anchor =
        camera_page->addNew<widgets::page_anchor>(*blueprint_page);
    auto *to_camera_anchor =
        blueprint_page->addNew<widgets::page_anchor>(*camera_page);
    to_camera_anchor->setPositionScheme(Wt::PositionScheme::Absolute);
    to_blueprint_anchor->setPositionScheme(Wt::PositionScheme::Absolute);
    to_camera_anchor->setOffsets(top, Wt::Side::Top);
    to_camera_anchor->setOffsets(left, Wt::Side::Left);
    to_blueprint_anchor->setOffsets(top, Wt::Side::Top);
    to_blueprint_anchor->setOffsets(left, Wt::Side::Left);

    mqtt.incoming().connect(&domestic_supervisor::perform_state_change_mqtt,
                            this);
    mqtt.incoming().connect(&domestic_supervisor::receive_image, this);
    internalPathChanged().connect(this,
                                  &domestic_supervisor::perform_path_change);
    blueprint_page->device_clicked().connect(
        &domestic_supervisor::send_state_change_mqtt, this);
    deepnet.processed().connect(&domestic_supervisor::perform_image_swap, this);
    deepnet.changed_num_detections().connect(
        &domestic_supervisor::perform_deepnet_num_change, this);
    camera_page->mark_faces_changed().connect(
        &domestic_supervisor::perform_deepnet_state_change, this);

    deepnet.set_enabled();
    deepnet.set_processing();
}

domestic_supervisor::~domestic_supervisor()
{
    mqtt.incoming().disconnect(this);
    deepnet.processed().disconnect(this);
    deepnet.changed_num_detections().disconnect(this);
}

void domestic_supervisor::perform_path_change(std::string const &ctx)
{
    if (ctx == camera_page->path()) {
        deepnet.set_enabled();
        container->setCurrentIndex(0);
    }
    else if (ctx == blueprint_page->path()) {
        deepnet.reset_enabled();
        container->setCurrentIndex(1);
    }
    else {
        Wt::WApplication::instance()->setInternalPath(
            std::string(camera_page->path()), false);
    }
}

void domestic_supervisor::perform_state_change_mqtt(
    controllers::mqtt::incoming_event const &ctx)
{
    if (ctx.topic() == ack_topic) {
        try {
            int dev_number = std::atoi(ctx.message().data());
            if (dev_number < 0 || dev_number >= device_names.size()) {
                throw std::out_of_range(std::to_string(dev_number));
            }

            if (UpdateLock l{this}) {
                blueprint_page->redraw_state(dev_number);
                triggerUpdate();
            }
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

void domestic_supervisor::send_state_change_mqtt(std::size_t const &ctx)
{
    mqtt.publish_message(std::string(state_topic), std::to_string(ctx), 2);
}

void domestic_supervisor::perform_image_swap(
    controllers::deepnet::image const &ctx)
{
    if (UpdateLock l{this}) {
        camera_page->set_image(ctx);
        triggerUpdate();
    }
}

void domestic_supervisor::receive_image(
    controllers::mqtt::incoming_event const &ctx)
{
    if (ctx.topic() == cam_topic && deepnet.state_enabled()) {
        deepnet.enqueue(std::string(ctx.message()));
    }
}

void domestic_supervisor::perform_deepnet_state_change()
{
    deepnet.toggle_processing();
}

void domestic_supervisor::perform_deepnet_num_change(int const &ctx)
{
    if (UpdateLock l{this}) {
        camera_page->set_num_detections(ctx);
        triggerUpdate();
    }
}
