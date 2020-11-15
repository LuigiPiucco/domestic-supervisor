#pragma once

#include <string>

/// Models physical system-controllable apparatus, of with there are two
/// types: lights and locks.
class device {
public:
    /// Enumerates the possible things a device could be.
    enum class types {
        light_bulb,
        lock,
    };

    constexpr device(char const *name, device::types type) noexcept
        : _state(false), _name(name), _type(type)
    {
    }
    constexpr device(device const &d) = default;
    device(device &&d) = default;
    auto operator=(device const &rhs) -> device & = default;
    auto operator=(device &&rhs) -> device & = default;
    constexpr ~device() = default;

    /// Activate this device.
    void set() noexcept;
    /// Deactivate this device.
    void reset() noexcept;
    /// Toggle this device.
    void toggle() noexcept;
    /// Get device's state.
    [[nodiscard]] auto state() const noexcept -> bool;

    /// Get device's name.
    [[nodiscard]] auto name() const noexcept -> char const *;
    /// Get device's type.
    [[nodiscard]] auto type() const noexcept -> types const &;

private:
    bool _state;

    char const *_name;
    types _type;
};
