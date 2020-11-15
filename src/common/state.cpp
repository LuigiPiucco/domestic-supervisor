#include "state.hpp"

controllers::state::state(std::initializer_list<device> l) noexcept
    : _devices(l)
{
}

void controllers::state::append_devices(std::initializer_list<device> l)
{
    for (std::scoped_lock lock{device_mutex}; auto const &e : l) {
        _devices.emplace_back(e);
    }
}

void controllers::state::remove_devices(std::initializer_list<std::size_t> l)
{
    for (std::scoped_lock lock{device_mutex}; auto const &i : l) {
        _devices.erase(std::next(_devices.cbegin(), i));
    }
}

auto controllers::state::devices() const noexcept
    -> controllers::state::device_list const &
{
    std::scoped_lock l{device_mutex};
    return _devices;
}

void controllers::state::set(std::size_t i)
{
    std::scoped_lock l{device_mutex};
    _devices.at(i).set();
    _changed(std::make_pair(i, true));
}

void controllers::state::reset(std::size_t i)
{
    std::scoped_lock l{device_mutex};
    _devices.at(i).reset();
    _changed(std::make_pair(i, false));
}

void controllers::state::toggle(std::size_t i)
{
    std::scoped_lock l{device_mutex};
    _devices.at(i).toggle();
    _changed(std::make_pair(i, _devices.at(i).state()));
}

auto controllers::state::changed() noexcept
    -> utils::event<std::pair<std::size_t, bool>> &
{
    return _changed;
}
