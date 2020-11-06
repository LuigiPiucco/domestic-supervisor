#include "device.hpp"

constexpr device::device(std::string_view name, std::string_view location,
                         std::string_view description, enum device::type type)
    : _name(name), _location(location), _description(description), _type(type),
      _state(false)
{
}

constexpr device::device(std::string_view &&name, std::string_view &&location,
                         std::string_view &&description,
                         enum device::type &&type)
    : _name(name), _location(location), _description(description), _type(type),
      _state(false)
{
}

void device::set() noexcept { _state = true; }

void device::reset() noexcept { _state = false; }

void device::toggle() noexcept { _state = !_state; }

auto device::state() const noexcept -> bool { return _state; };

auto device::name() const noexcept -> std::string_view const & { return _name; }

auto device::location() const noexcept -> std::string_view const &
{
    return _location;
}

auto device::description() const noexcept -> std::string_view const &
{
    return _description;
}

auto device::type() const noexcept -> enum device::type const &
{
    return _type;
}
