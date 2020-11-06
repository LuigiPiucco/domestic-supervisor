#include "state.hpp"
#include <iterator>

controllers::state::state(std::initializer_list<device> l) : _devices(l) {}

void controllers::state::append_devices(std::initializer_list<device> l)
{
    _devices.emplace_back(l);
}

void controllers::state::remove_devices(std::initializer_list<std::size_t> l)
{
    for (auto const &i : l) {
        _devices.erase(std::next(_devices.cbegin(), i));
    }
}

auto controllers::state::devices() const noexcept
    -> controllers::state::device_list const &
{
    return _devices;
}

void controllers::state::set(std::size_t i) { _devices.at(i).set(); }

void controllers::state::reset(std::size_t i) { _devices.at(i).reset(); }

void controllers::state::toggle(std::size_t i) { _devices.at(i).toggle(); }

auto controllers::state::changed() noexcept -> utils::event<device> &
{
    return _changed;
}
