#include "device.hpp"

void device::set() noexcept { _state = true; }

void device::reset() noexcept { _state = false; }

void device::toggle() noexcept { _state = !_state; }

auto device::state() const noexcept -> bool { return _state; };

auto device::name() const noexcept -> char const * { return _name; }

auto device::type() const noexcept -> device::types const & { return _type; }
