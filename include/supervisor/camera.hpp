#pragma once

#include "camera_panel.hpp"
#include "deepnet.hpp"
#include "dynamic_image.hpp"
#include "event.hpp"
#include "view.hpp"
#include <Wt/WImage.h>
#include <boost/iostreams/stream.hpp>
#include <memory>

namespace views {
    class camera final : public view {
    public:
        [[nodiscard]] constexpr auto name() const -> std::string_view final
        {
            return "Câmera";
        }
        [[nodiscard]] constexpr auto path() const -> std::string_view final
        {
            return "/camera";
        }

        camera();

        void set_image(controllers::deepnet::image const &im);
        [[nodiscard]] auto mark_faces_changed() noexcept
            -> utils::event<bool> &;
        void set_num_detections(int num);

    private:
        Wt::WImage *center_image;
        Wt::WImage *left_image;
        Wt::WImage *right_image;

        std::shared_ptr<dynamic_image> image_resource;

        widgets::camera_panel *panel;
    };
} // namespace views