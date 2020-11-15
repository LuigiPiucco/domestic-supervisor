#pragma once

#include <Wt/WColor.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WLink.h>
#include <Wt/WPushButton.h>
#include <mutex>

namespace widgets {
    class icon_button : public Wt::WPushButton {
    public:
        icon_button(char const *active_image_path,
                    char const *inactive_image_path, Wt::WLength side_length);

        void set();
        void reset();
        void toggle();

    private:
        Wt::WLink const active_image;
        Wt::WLink const inactive_image;
        bool active;
        std::mutex self_mutex;
    };
} // namespace widgets
