#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-utils/include/math.hpp"

#include <cmath>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::utils {

// ============================================================================
// Linear
// ----------------------------------------------------------------------------

/**
 * @brief No easing, linear interpolation.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return t unchanged
 */
template<Float T>
[[nodiscard]] constexpr T linear(
    T const t
    ) {

    return t;
}

// ============================================================================
// Quadratic
// ----------------------------------------------------------------------------

/**
 * @brief Quadratic ease-in, slow start then accelerate.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_quad(
    T const t
    ) {

    return square(t);
}

/**
 * @brief Quadratic ease-out, fast start then decelerate.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T out_quad(
    T const t
    ) {

    return T(1) - square(T(1) - t);
}

/**
 * @brief Quadratic ease-in-out, slow start and end with fast middle.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_out_quad(
    T const t
    ) {

    return t < T(0.5)
        ? T(2) * square(t)
        : T(1) - T(2) * square(T(1) - t);
}

// ============================================================================
// Cubic
// ----------------------------------------------------------------------------

/**
 * @brief Cubic ease-in, very slow start then strong acceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_cubic(
    T const t
    ) {

    return t * square(t);
}

/**
 * @brief Cubic ease-out, fast start then strong deceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T out_cubic(
    T const t
    ) {

    T const u = T(1) - t;
    return T(1) - u * square(u);
}

/**
 * @brief Cubic ease-in-out, slow start and end with sharp middle transition.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_out_cubic(
    T const t
    ) {

    return t < T(0.5)
        ? T(4) * t * square(t)
        : T(1) - T(0.5) * std::pow(T(-2) * t + T(2), T(3));
}

// ============================================================================
// Quartic
// ----------------------------------------------------------------------------

/**
 * @brief Quartic ease-in, extremely slow start with very strong acceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_quart(
    T const t
    ) {

    return square(square(t));
}

/**
 * @brief Quartic ease-out, fast start with very strong deceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T out_quart(
    T const t
    ) {

    T const u = T(1) - t;
    return T(1) - square(square(u));
}

/**
 * @brief Quartic ease-in-out, very slow start and end with aggressive middle.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_out_quart(
    T const t
    ) {

    return t < T(0.5)
        ? T(8) * square(square(t))
        : T(1) - T(0.5) * std::pow(T(-2) * t + T(2), T(4));
}

// ============================================================================
// Sine
// ----------------------------------------------------------------------------

/**
 * @brief Sinusoidal ease-in, gentle curved acceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_sine(
    T const t
    ) {

    return T(1) - std::cos(t * static_cast<T>(HALF_PI));
}

/**
 * @brief Sinusoidal ease-out, gentle curved deceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T out_sine(
    T const t
    ) {

    return std::sin(t * static_cast<T>(HALF_PI));
}

/**
 * @brief Sinusoidal ease-in-out, smooth S-curve motion.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_out_sine(
    T const t
    ) {

    return T(-0.5) * (std::cos(static_cast<T>(PI) * t) - T(1));
}

// ============================================================================
// Exponential
// ----------------------------------------------------------------------------

/**
 * @brief Exponential ease-in, extremely slow start with explosive acceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_expo(
    T const t
    ) {

    return t <= T(0) ? T(0) : std::pow(T(2), T(10) * (t - T(1)));
}

/**
 * @brief Exponential ease-out, fast start with rapid deceleration.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T out_expo(
    T const t
    ) {

    return t >= T(1) ? T(1) : T(1) - std::pow(T(2), T(-10) * t);
}

/**
 * @brief Exponential ease-in-out, extremely slow start and end with explosive middle.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value
 */
template<Float T>
[[nodiscard]] constexpr T in_out_expo(
    T const t
    ) {

    if (t <= T(0)) { return T(0); }
    if (t >= T(1)) { return T(1); }

    return t < T(0.5)
        ? T(0.5) * std::pow(T(2), T(20) * t - T(10))
        : T(1) - T(0.5) * std::pow(T(2), T(-20) * t + T(10));
}

// ============================================================================
// Back
// ----------------------------------------------------------------------------

/**
 * @brief Back ease-in, pulls backward before moving forward (overshoots start).
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value that dips below 0 before accelerating
 */
template<Float T>
[[nodiscard]] constexpr T in_back(
    T const t
    ) {

    T constexpr c = T(1.70158);
    return (c + T(1)) * t * square(t) - c * square(t);
}

/**
 * @brief Back ease-out, overshoots target then settles back.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value that exceeds 1 before settling
 */
template<Float T>
[[nodiscard]] constexpr T out_back(
    T const t
    ) {

    T constexpr c = T(1.70158);
    T const u = t - T(1);
    return T(1) + (c + T(1)) * u * square(u) + c * square(u);
}

// ============================================================================
// Bounce
// ----------------------------------------------------------------------------

/**
 * @brief Bounce ease-out, simulates bouncing ball settling.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value with decreasing bounces
 */
template<Float T>
[[nodiscard]] constexpr T out_bounce(
    T const t
    ) {

    T constexpr n1 = T(7.5625);
    T constexpr d1 = T(2.75);

    if (t < T(1) / d1) {

        return n1 * square(t);
    } else if (t < T(2) / d1) {

        T const t2 = t - T(1.5) / d1;
        return n1 * square(t2) + T(0.75);
    } else if (t < T(2.5) / d1) {

        T const t2 = t - T(2.25) / d1;
        return n1 * square(t2) + T(0.9375);
    } else {

        T const t2 = t - T(2.625) / d1;
        return n1 * square(t2) + T(0.984375);
    }
}

/**
 * @brief Bounce ease-in, inverted bounce effect at start.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value with increasing bounces
 */
template<Float T>
[[nodiscard]] constexpr T in_bounce(
    T const t
    ) {

    return T(1) - out_bounce(T(1) - t);
}

/**
 * @brief Bounce ease-in-out, bounces at both start and end.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value with bounces at extremes
 */
template<Float T>
[[nodiscard]] constexpr T in_out_bounce(
    T const t
    ) {

    return t < T(0.5)
        ? T(0.5) - T(0.5) * out_bounce(T(1) - T(2) * t)
        : T(0.5) + T(0.5) * out_bounce(T(2) * t - T(1));
}

// ============================================================================
// Elastic
// ----------------------------------------------------------------------------

/**
 * @brief Elastic ease-in, spring-like wobble before accelerating.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value with oscillation overshooting start
 */
template<Float T>
[[nodiscard]] constexpr T in_elastic(
    T const t
    ) {

    if (t <= T(0)) { return T(0); }
    if (t >= T(1)) { return T(1); }

    T constexpr c4 = (T(2) * static_cast<T>(PI)) / T(3);

    return -std::pow(T(2), T(10) * t - T(10)) * std::sin((t * T(10) - T(10.75)) * c4);
}

/**
 * @brief Elastic ease-out, spring-like wobble when settling at target.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value with oscillation overshooting end
 */
template<Float T>
[[nodiscard]] constexpr T out_elastic(
    T const t
    ) {

    if (t <= T(0)) { return T(0); }
    if (t >= T(1)) { return T(1); }

    T constexpr c4 = (T(2) * static_cast<T>(PI)) / T(3);

    return std::pow(T(2), T(-10) * t) * std::sin((t * T(10) - T(0.75)) * c4) + T(1);
}

/**
 * @brief Elastic ease-in-out, spring wobbles at both extremes.
 * @tparam T Floating point type
 * @param t Normalized time
 * @return Eased value with oscillations at start and end
 */
template<Float T>
[[nodiscard]] constexpr T in_out_elastic(
    T const t
    ) {

    if (t <= T(0)) { return T(0); }
    if (t >= T(1)) { return T(1); }

    T constexpr c5 = (T(2) * static_cast<T>(PI)) / T(4.5);

    return t < T(0.5)
        ? -(std::pow(T(2), T(20) * t - T(10)) * std::sin((T(20) * t - T(11.125)) * c5)) / T(2)
        : (std::pow(T(2), T(-20) * t + T(10)) * std::sin((T(20) * t - T(11.125)) * c5)) / T(2) + T(1);
}

} // namespace titan::utils
