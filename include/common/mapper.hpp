#pragma once

#include "constants.hpp"
#include <algorithm>
#include <cmath>
#include <string_view>
#include <tuple>

constexpr auto map_position(char const *name) -> std::tuple<double, double>
{
    // It's okay to compare pointers, as the names are static and, therefore,
    // unique and all living through the program.
    if (name == device_names[0]) {
        return {557.0, 48.0};
    }
    if (name == device_names[1]) {
        return {533.0, 87.0};
    }
    if (name == device_names[2]) {
        return {418.0, 122.0};
    }
    if (name == device_names[3]) {
        return {164.0, 178.0};
    }
    if (name == device_names[4]) {
        return {777.0, 158.0};
    }
    if (name == device_names[5]) {
        return {942.0, 193.0};
    }
    if (name == device_names[6]) {
        return {430.0, 272.0};
    }
    if (name == device_names[7]) {
        return {557.0, 264.0};
    }
    if (name == device_names[8]) {
        return {1070.0, 336.0};
    }
    if (name == device_names[9]) {
        return {329.0, 346.0};
    }
    if (name == device_names[10]) {
        return {284.0, 394.0};
    }
    if (name == device_names[11]) {
        return {477.0, 374.0};
    }
    if (name == device_names[12]) {
        return {530.0, 442.0};
    }
    if (name == device_names[13]) {
        return {839.0, 426.0};
    }
    if (name == device_names[14]) {
        return {142.0, 520.0};
    }
    if (name == device_names[15]) {
        return {455.0, 624.0};
    }
    if (name == device_names[16]) {
        return {686.0, 671.0};
    }
    if (name == device_names[17]) {
        return {831.0, 700.0};
    }

    // This should never happen, unless we forget to list some device in
    // device_names.
    return {std::nan("You passed an unknown name to map_position."),
            std::nan("You passed an unknown name to map_position.")};
}
