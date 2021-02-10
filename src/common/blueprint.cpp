#include "blueprint.hpp"

views::blueprint::blueprint(std::vector<device> const &devices) : buttons()
{
    setPositionScheme(Wt::PositionScheme::Relative);

    constexpr auto width = 1160;
    constexpr auto height = 864;

    resize(width, height);
    Wt::WLength const full{100, Wt::LengthUnit::Percentage};
    addCssRule("#" + id(), "margin: calc(50vh - " + std::to_string(height / 2) +
                               "px) auto;");
    addCssRule(".side-image-" + id(),
               "opacity: 0.5; backdrop-filter: blur(32px);");

    // Here we create the 3 images, and since they dont't need to be referenced
    // afterward, they're scope local.
    Wt::WLink bg("assets/blueprint.jpg");
    auto *background_image = addNew<Wt::WImage>(bg);
    background_image->resize(width, height);
    background_image = addNew<Wt::WImage>(bg);
    background_image->resize(width, height);
    background_image->setPositionScheme(Wt::PositionScheme::Absolute);
    background_image->setOffsets(0, Wt::Side::Top);
    background_image->setOffsets(full, Wt::Side::Right);
    background_image->addStyleClass("side-image-" + id());
    background_image = addNew<Wt::WImage>(bg);
    background_image->resize(width, height);
    background_image->setPositionScheme(Wt::PositionScheme::Absolute);
    background_image->setOffsets(0, Wt::Side::Top);
    background_image->setOffsets(full, Wt::Side::Left);
    background_image->addStyleClass("side-image-" + id());

    for (std::size_t i = 0; auto const &dev : devices) {
        auto const *active = dev.type() == device::types::lock
                                 ? locked_padlock_path
                                 : lit_light_bulb_path;
        auto const *inactive = dev.type() == device::types::lock
                                   ? unlocked_padlock_path
                                   : unlit_light_bulb_path;
        auto const side_length = dev.type() == device::types::lock ? 32 : 64;
        buttons.at(i) =
            addNew<widgets::icon_button>(active, inactive, side_length);
        buttons.at(i)->setPositionScheme(Wt::PositionScheme::Absolute);
        auto const &[x, y] = map_position(dev.name());
        buttons.at(i)->setOffsets(x, Wt::Side::Left);
        buttons.at(i)->setOffsets(y, Wt::Side::Top);
        buttons.at(i)->clicked().connect([this, i]() { _device_clicked(i); });

        ++i;
    }
}

void views::blueprint::redraw_state(std::size_t dev)
{
    buttons.at(dev)->toggle();
}

auto views::blueprint::device_clicked() noexcept -> utils::event<std::size_t> &
{
    return _device_clicked;
}
