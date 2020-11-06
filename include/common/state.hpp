#pragma once

#include "device.hpp"
#include "event.hpp"
#include <initializer_list>
#include <string>
#include <vector>

namespace controllers {
    /// Manages the state of the device collection.
    class state {
    public:
        using device_list = std::vector<device>;

        state() = delete;
        /// List initialization constructor.
        ///
        /// Example:
        /// \verbatim embed:rst:leading-slashes
        /// .. code-block:: c++
        ///     :linenos:
        ///
        ///     device d1 = device(/* Elided... */);
        ///     device d2 = device(/* Elided... */);
        ///     controllers::state s1 = {d1, d2}; // or
        ///     controllers::state s2{d1, d2} // or
        ///     auto s3 = controllers::state{d1, d2};
        /// \verbatim
        state(std::initializer_list<device> l);

        /// Append all elements in a initializer list.
        void append_devices(std::initializer_list<device> l);
        /// Remove elements whose index is in `l`.
        ///
        /// Throws if out-of-range index is listed.
        void remove_devices(std::initializer_list<std::size_t> l);
        /// Get the device list.
        [[nodiscard]] auto devices() const noexcept -> device_list const &;

        /// Activates one device.
        void set(std::size_t i);
        /// Deactivates one device.
        void reset(std::size_t i);
        /// Toggles one device.
        void toggle(std::size_t i);

        /// Get the `event` for the change action.
        [[nodiscard]] auto changed() noexcept -> utils::event<device> &;

    private:
        device_list _devices;
        utils::event<device> _changed;
    };
} // namespace controllers
