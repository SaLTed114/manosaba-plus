// Utils/MathUtils.h
#ifndef UTILS_MATHUTILS_H
#define UTILS_MATHUTILS_H

#include <cmath>
#include <algorithm>

namespace Salt2D::Utils {

/// <summary>
/// Ensures a float value is finite and within a valid range.
/// If the value is NaN, Inf, or less than minValue, it returns minValue.
/// </summary>
/// <param name="value">The value to validate</param>
/// <param name="minValue">The minimum valid value (default: 1.0f)</param>
/// <returns>A valid finite value >= minValue</returns>
inline float EnsureFinite(float value, float minValue = 1.0f) {
    if (!std::isfinite(value) || value < minValue) {
        return minValue;
    }
    return value;
}

/// <summary>
/// Clamps and wraps an integer value within range [0, n).
/// If n <= 0, returns 0.
/// Handles negative values by wrapping them to positive range.
/// </summary>
/// <param name="v">The value to clamp/wrap</param>
/// <param name="n">The upper bound (exclusive)</param>
/// <returns>Value in range [0, n)</returns>
inline int ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

/// <summary>
/// Clamps a value between min and max (inclusive).
/// </summary>
template<typename T>
inline T Clamp(T value, T minValue, T maxValue) {
    return (std::max)(minValue, (std::min)(value, maxValue));
}

} // namespace Salt2D::Utils

#endif // UTILS_MATHUTILS_H
