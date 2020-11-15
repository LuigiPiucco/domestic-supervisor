#include "camera.hpp"
#include <memory>

views::camera::camera() : image_resource(std::make_shared<dynamic_image>())
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

    center_image = addNew<Wt::WImage>(Wt::WLink(image_resource));
    center_image->resize(width, height);
    left_image = addNew<Wt::WImage>(Wt::WLink(image_resource));
    left_image->resize(width, height);
    left_image->setPositionScheme(Wt::PositionScheme::Absolute);
    left_image->setOffsets(0, Wt::Side::Top);
    left_image->setOffsets(full, Wt::Side::Right);
    left_image->addStyleClass("side-image-" + id());
    right_image = addNew<Wt::WImage>(Wt::WLink(image_resource));
    right_image->resize(width, height);
    right_image->setPositionScheme(Wt::PositionScheme::Absolute);
    right_image->setOffsets(0, Wt::Side::Top);
    right_image->setOffsets(full, Wt::Side::Left);
    right_image->addStyleClass("side-image-" + id());

    constexpr auto top = 32;
    constexpr auto left = 32;

    panel = addNew<widgets::camera_panel>();
    panel->setOffsets(top, Wt::Side::Top);
    panel->setOffsets(left, Wt::Side::Left);
}

void views::camera::set_image(controllers::deepnet::image const &im)
{
    image_resource->set_image(im);
    center_image->setImageLink(Wt::WLink(image_resource));
    left_image->setImageLink(Wt::WLink(image_resource));
    right_image->setImageLink(Wt::WLink(image_resource));
}

auto views::camera::mark_faces_changed() noexcept -> utils::event<bool> &
{
    return panel->mark_faces_changed();
}

void views::camera::set_num_detections(int num)
{
    panel->set_num_detections(num);
}