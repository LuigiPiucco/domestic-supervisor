#pragma once
// `string_view` is as lightweight as `char *`, and it is `constexpr`, meaning
// no run-time overhead. The main reason to prefer it over C-style `char *` is
// that it has some of the niceties of the `string` interface.
//
// And why not just `string`? Because that one implies allocation, and that's
// not `constexpr`. Also, `string_view` is non-owning, so no management is
// needed from `device`.
#include <string_view>

/// Models physical system-controllable apparatus, of with there are two
/// types: lights and locks.
class device {
public:
    /// Enumerates the possible things a device could be.
    enum class type {
        light_bulb,
        lock,
    };

    /// May not be default constructed; What would go in the immutable members?
    device() = delete;
    /// Copy arguments constructor.
    ///
    /// `constexpr`, therefore may produce compile-time constants.
    constexpr device(std::string_view name, std::string_view location,
                     std::string_view description, device::type type);
    /// Move arguments constructor.
    ///
    /// `constexpr`, therefore may produce compile-time constants.
    constexpr device(std::string_view &&name, std::string_view &&location,
                     std::string_view &&description, device::type &&type);

    /// Activate this device.
    void set() noexcept;
    /// Deactivate this device.
    void reset() noexcept;
    /// Toggle this device.
    void toggle() noexcept;
    /// Get device's state.
    [[nodiscard]] auto state() const noexcept -> bool;

    /// Get device's name.
    [[nodiscard]] auto name() const noexcept -> std::string_view const &;
    /// Get device's location.
    [[nodiscard]] auto location() const noexcept -> std::string_view const &;
    /// Get device's description.
    [[nodiscard]] auto description() const noexcept -> std::string_view const &;
    /// Get device's type.
    [[nodiscard]] auto type() const noexcept -> device::type const &;

private:
    bool _state;

    std::string_view const _name;
    std::string_view const _location;
    std::string_view const _description;
    enum type const _type;
};
