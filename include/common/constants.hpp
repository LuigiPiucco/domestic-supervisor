#pragma once

#include "device.hpp"
#include <array>
#include <string_view>

constexpr std::array<const char *, 18> device_names = {
    "Storage lock",         "Storage light",     "Master bathroom light",
    "Master bedroom light", "Kitchen light",     "Dining light",
    "Bath tub light",       "Bathroom 2 light",  "Side door lock",
    "Master bedroom lock",  "Bedroom 2 lock",    "Hall light",
    "Bedroom 3 lock",       "Family room light", "Bedroom 2 light",
    "Bedroom 3 light",      "Front door lock",   "Porch light",
};

constexpr std::array<device, 18> devices = {
    device{device_names[0], device::types::lock},
    device{device_names[1], device::types::light_bulb},
    device{device_names[2], device::types::light_bulb},
    device{device_names[3], device::types::light_bulb},
    device{device_names[4], device::types::light_bulb},
    device{device_names[5], device::types::light_bulb},
    device{device_names[6], device::types::light_bulb},
    device{device_names[7], device::types::light_bulb},
    device{device_names[8], device::types::lock},
    device{device_names[9], device::types::lock},
    device{device_names[10], device::types::lock},
    device{device_names[11], device::types::light_bulb},
    device{device_names[12], device::types::lock},
    device{device_names[13], device::types::light_bulb},
    device{device_names[14], device::types::light_bulb},
    device{device_names[15], device::types::light_bulb},
    device{device_names[16], device::types::lock},
    device{device_names[17], device::types::light_bulb},
};
