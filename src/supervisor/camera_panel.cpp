#include "camera_panel.hpp"

widgets::camera_panel::camera_panel()
{
    addCssRule("#" + id(), "background: rgba(139, 233, 253, 0.5); "
                           "backdrop-filter: blur(32px); border-radius: 32px;");

    setPositionScheme(Wt::PositionScheme::Absolute);

    constexpr auto top = 32;
    constexpr auto left = 417;
    constexpr auto width = 218;
    setOffsets(top, Wt::Side::Top);
    setOffsets(left, Wt::Side::Left);

    setWidth(width);

    setContentAlignment(Wt::AlignmentFlag::Center);
    setVerticalAlignment(Wt::AlignmentFlag::Middle);

    addNew<Wt::WText>("Controles");

    addNew<Wt::WBreak>();

    auto *mark_faces_checkbox = addNew<Wt::WCheckBox>("Marcar rostos");
    mark_faces_checkbox->setChecked();
    mark_faces_checkbox->changed().connect([this, mark_faces_checkbox]() {
        _mark_faces_changed(mark_faces_checkbox->isChecked());
    });

    std::scoped_lock l{num_detections_box_mutex};
    num_detections_box = addNew<Wt::WSpinBox>();
    num_detections_box->setValue(0);
    num_detections_box->setReadOnly(true);
    num_detections_box->setInline(true);
    addNew<Wt::WText>("Número de rostos");
}

auto widgets::camera_panel::mark_faces_changed() noexcept
    -> utils::event<bool> &
{
    return _mark_faces_changed;
}

void widgets::camera_panel::set_num_detections(int num)
{
    std::scoped_lock l{num_detections_box_mutex};
    num_detections_box->setValue(num);
}
