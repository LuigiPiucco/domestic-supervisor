#pragma once

#include "event.hpp"
#include <Wt/WBreak.h>
#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WSpinBox.h>
#include <Wt/WText.h>
#include <mutex>

namespace widgets {
    class camera_panel : public Wt::WContainerWidget {
    public:
        camera_panel();

        [[nodiscard]] auto mark_faces_changed() noexcept
            -> utils::event<bool> &;
        void set_num_detections(int num);

    private:
        utils::event<bool> _mark_faces_changed;
        std::mutex num_detections_box_mutex;
        Wt::WSpinBox *num_detections_box;
    };
} // namespace widgets