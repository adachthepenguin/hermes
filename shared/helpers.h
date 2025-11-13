#pragma once

namespace Math {

template <typename T>
T Clamp(T value, T lowest, T highest) {
    if (value < lowest) {
        return lowest;
    } else if (value > highest) {
        return highest;
    } else {
        return value;
    }
}

} // namespace Math
