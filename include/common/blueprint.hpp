#pragma once

#include "constants.hpp"
#include "device.hpp"
#include "event.hpp"
#include "icon_button.hpp"
#include "mapper.hpp"
#include "view.hpp"
#include <Wt/WImage.h>
#include <Wt/WLength.h>
#include <Wt/WLink.h>
#include <array>
#include <memory>

namespace views {
    class blueprint final : public view {
    public:
        static constexpr char const *lit_light_bulb_path =
            "assets/lit-lightbulb.svg";
        static constexpr char const *unlit_light_bulb_path =
            "assets/unlit-lightbulb.svg";
        static constexpr char const *locked_padlock_path =
            "assets/locked-padlock.svg";
        static constexpr char const *unlocked_padlock_path =
            "assets/unlocked-padlock.svg";

        blueprint();

        [[nodiscard]] constexpr auto name() const -> std::string_view final
        {
            return "Planta baixa";
        }
        [[nodiscard]] constexpr auto path() const -> std::string_view final
        {
            return "/blueprint";
        }

        void redraw_state(std::size_t dev);
        [[nodiscard]] auto device_clicked() noexcept
            -> utils::event<std::size_t> &;

    private:
        std::array<widgets::icon_button *, device_names.size()> buttons;
        utils::event<std::size_t> _device_clicked;
    };
} // namespace views
