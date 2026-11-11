#pragma once
#include <cstddef>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
// Shared existing classic-locale precision17 CSV upper bound. Counts are
// reservations before a group, not measured bytes or a wall-time attestation.
inline std::size_t CompositePrimaryByteReservation(std::size_t inputs,std::size_t channels,std::size_t samples) {
    const auto require=[](bool v){if(!v)throw std::invalid_argument("composite_primary_reservation_overflow");};
    const auto max=std::numeric_limits<std::size_t>::max();
    require(channels<=(max-67)/33);const auto row=33*(channels+2)+1;
    require(channels<=(max-16)/32);const auto header=16+32*channels;
    require(inputs<=max-2 && inputs+2<=max/header);const auto headers=(inputs+2)*header;
    require(samples<=(max-headers)/row);return headers+samples*row;
}
}
