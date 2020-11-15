#include "icon_button.hpp"

widgets::icon_button::icon_button(char const *active_image_path,
                                  char const *inactive_image_path,
                                  Wt::WLength side_length)
    : active_image(Wt::WLink(active_image_path)),
      inactive_image(Wt::WLink(inactive_image_path)), active(false)
{
    addCssRule("#" + id(),
               "background: rgba(139, 233, 253, 0.25); border: none; "
               "backdrop-filter: blur(32px); border-radius: 12px;");

    resize(side_length, side_length);

    reset();
}

void widgets::icon_button::set()
{
    std::scoped_lock l{self_mutex};
    setIcon(active_image);
    active = true;
}

void widgets::icon_button::reset()
{
    std::scoped_lock l{self_mutex};
    setIcon(inactive_image);
    active = false;
}

void widgets::icon_button::toggle()
{
    std::scoped_lock l{self_mutex};
    active = !active;
    if (active) {
        setIcon(active_image);
    }
    else {
        setIcon(inactive_image);
    }
}