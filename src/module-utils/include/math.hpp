#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics.hpp"

#include <cmath>
#include <algorithm>
#include <numbers>
#include <concepts>
#include <optional>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::utils {

// ============================================================================
// Constants
// ----------------------------------------------------------------------------

inline float constexpr PI      = std::numbers::pi_v<float>; ///< Pi constant
inline float constexpr TAU     = 2.0f * PI; ///< Tau constant (2π)
inline float constexpr HALF_PI = 0.5f * PI; ///< Half pi (π/2)
inline float constexpr DEG2RAD = PI / 180.0f; ///< Degrees to radians conversion factor
inline float constexpr RAD2DEG = 180.0f / PI; ///< Radians to degrees conversion factor
inline float constexpr EPSILON = 1.0e-6f; ///< Small epsilon for floating point comparisons

// ============================================================================
// Concepts
// ----------------------------------------------------------------------------

/**
 * @brief Any numeric type (int, float, double, etc.)
 */
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

/**
 * @brief Any floating point type (float, double, long double)
 */
template<typename T>
concept Float = std::floating_point<T>;

/**
 * @brief Any type that has .x and .y members
 */
template<typename T>
concept Vector2Like = requires(T const v) {

    { v.x };
    { v.y };
};

/**
 * @brief Any type that has .position and .size members
 */
template<typename T>
concept RectLike = requires(T const r) {

    { r.position };
    { r.size };
};

// ============================================================================
// Length and distance
// ----------------------------------------------------------------------------

/**
 * @brief Calculate squared length of a vector.
 * @param v Vector to measure
 * @return Squared length
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float length_sq(
    Vec const v
    ) {

    return static_cast<float>(v.x) * static_cast<float>(v.x) + static_cast<float>(v.y) * static_cast<float>(v.y);
}

/**
 * @brief Calculate length (magnitude) of a vector.
 * @param v Vector to measure
 * @return Length of the vector
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float length(
    Vec const v
    ) {

    return std::sqrt(length_sq(v));
}

/**
 * @brief Calculate squared distance between two points.
 * @param a First point
 * @param b Second point
 * @return Squared distance
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float distance_sq(
    Vec const a,
    Vec const b
    ) {

    return length_sq(b - a);
}

/**
 * @brief Calculate distance between two points.
 * @param a First point
 * @param b Second point
 * @return Distance between points
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float distance(
    Vec const a,
    Vec const b
    ) {

    return std::sqrt(distance_sq(a, b));
}

// ============================================================================
// Dot and cross products
// ----------------------------------------------------------------------------

/**
 * @brief Calculate dot product of two vectors.
 * @param a First vector
 * @param b Second vector
 * @return Dot product
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float dot(
    Vec const a,
    Vec const b
    ) {

    return static_cast<float>(a.x) * static_cast<float>(b.x) + static_cast<float>(a.y) * static_cast<float>(b.y);
}

/**
 * @brief Calculate 2D cross product (z-component of 3D cross product).
 * @param a First vector
 * @param b Second vector
 * @return Cross product scalar
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float cross(
    Vec const a,
    Vec const b
    ) {

    return static_cast<float>(a.x) * static_cast<float>(b.y) - static_cast<float>(a.y) * static_cast<float>(b.x);
}

// ============================================================================
// Normalize
// ----------------------------------------------------------------------------

/**
 * @brief Normalize a vector to unit length.
 * @param v Vector to normalize
 * @return Normalized vector, or zero vector if length is too small
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f normalize(
    Vec const v
    ) {

    float const len = length(v);

    return (len > EPSILON)
        ? sf::Vector2f{static_cast<float>(v.x) / len, static_cast<float>(v.y) / len}
        : sf::Vector2f{0.0f, 0.0f};
}

// ============================================================================
// Rotation
// ----------------------------------------------------------------------------

/**
 * @brief Rotate a vector by an angle.
 * @param v Vector to rotate
 * @param angle Rotation angle
 * @return Rotated vector
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f rotate(
    Vec const v,
    sf::Angle const angle
    ) {

    float const c = std::cos(angle.asRadians());
    float const s = std::sin(angle.asRadians());

    return {
        static_cast<float>(v.x) * c - static_cast<float>(v.y) * s,
        static_cast<float>(v.x) * s + static_cast<float>(v.y) * c
        };
}

/**
 * @brief Rotate a vector by an angle in radians.
 * @param v Vector to rotate
 * @param angle_radians Rotation angle in radians
 * @return Rotated vector
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f rotate(
    Vec const v,
    float const angle_radians
    ) {

    float const c = std::cos(angle_radians);
    float const s = std::sin(angle_radians);

    return {
        static_cast<float>(v.x) * c - static_cast<float>(v.y) * s,
        static_cast<float>(v.x) * s + static_cast<float>(v.y) * c
        };
}

// ============================================================================
// Angles
// ----------------------------------------------------------------------------

/**
 * @brief Get the angle of a vector in radians.
 * @param v Vector
 * @return Angle in radians
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float angle_of(
    Vec const v
    ) {

    return std::atan2(static_cast<float>(v.y), static_cast<float>(v.x));
}

/**
 * @brief Calculate signed angle between two vectors.
 * @param a First vector
 * @param b Second vector
 * @return Angle in radians
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr float angle_between(
    Vec const a,
    Vec const b
    ) {

    return std::atan2(cross(a, b), dot(a, b));
}

/**
 * @brief Create a vector from an angle.
 * @param angle Angle
 * @param len Length of resulting vector
 * @return Vector pointing in the given direction
 */
template <Numeric N>
[[nodiscard]] constexpr sf::Vector2f from_angle(
    sf::Angle const angle,
    N const len = 1.0f
    ) {

    return {
        std::cos(static_cast<float>(angle.asRadians())) * static_cast<float>(len),
        std::sin(static_cast<float>(angle.asRadians())) * static_cast<float>(len)
        };
}

/**
 * @brief Create a vector from an angle in radians.
 * @param angle_radians Angle in radians
 * @param len Length of resulting vector
 * @return Vector pointing in the given direction
 */
template <Numeric N>
[[nodiscard]] constexpr sf::Vector2f from_angle(
    N const angle_radians,
    N const len = 1.0f
    ) {

    return {
        std::cos(static_cast<float>(angle_radians)) * static_cast<float>(len),
        std::sin(static_cast<float>(angle_radians)) * static_cast<float>(len)
        };
}

// ============================================================================
// Reflection and projection
// ----------------------------------------------------------------------------

/**
 * @brief Reflect a vector across a normal.
 * @param incident Incoming vector
 * @param normal Surface normal
 * @return Reflected vector
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f reflect(
    Vec const incident,
    Vec const normal
    ) {

    float const d = dot(incident, normal);

    return {
        static_cast<float>(incident.x) - 2.0f * d * static_cast<float>(normal.x),
        static_cast<float>(incident.y) - 2.0f * d * static_cast<float>(normal.y)
        };
}

/**
 * @brief Project a vector onto another vector.
 * @param v Vector to project
 * @param onto Vector to project onto
 * @return Projected vector
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f project(
    Vec const v,
    Vec const onto
    ) {

    float const d = dot(v, onto);
    float const len_sq = length_sq(onto);

    if (len_sq < EPSILON) { return {0.0f, 0.0f}; }

    float const scalar = d / len_sq;

    return {
        static_cast<float>(onto.x) * scalar,
        static_cast<float>(onto.y) * scalar
        };
}

// ============================================================================
// Perpendicular
// ----------------------------------------------------------------------------

/**
 * @brief Get perpendicular vector (counter-clockwise, 90 degrees).
 * @param v Input vector
 * @return Perpendicular vector
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f perpendicular(
    Vec const v
    ) {

    return {
        -static_cast<float>(v.y),
        static_cast<float>(v.x)
        };
}

/**
 * @brief Get perpendicular vector (clockwise, -90 degrees).
 * @param v Input vector
 * @return Perpendicular vector rotated clockwise
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f perpendicular_cw(
    Vec const v
    ) {

    return {
        static_cast<float>(v.y),
        -static_cast<float>(v.x)
        };
}

/**
 * @brief Get perpendicular vector (counter-clockwise, 90 degrees).
 * @param v Input vector
 * @return Perpendicular vector rotated counter-clockwise
 */
template <Vector2Like Vec>
[[nodiscard]] constexpr sf::Vector2f perpendicular_ccw(
    Vec const v
    ) {

    return {
        -static_cast<float>(v.y),
        static_cast<float>(v.x)
        };
}

// ============================================================================
// Clamping
// ----------------------------------------------------------------------------

/**
 * @brief Clamp a value between min and max.
 * @param value Value to clamp
 * @param lo Minimum value
 * @param hi Maximum value
 * @return Clamped value
 */
template<Numeric T>
[[nodiscard]] constexpr T clamp(
    T const value,
    T const lo,
    T const hi
    ) {

    return std::max(lo, std::min(hi, value));
}

/**
 * @brief Clamp a value between 0 and 1.
 * @param value Value to clamp
 * @return Clamped value in range [0, 1]
 */
template<Numeric T>
[[nodiscard]] constexpr T clamp01(
    T const value
    ) {

    return clamp(value, T(0), T(1));
}

// ============================================================================
// Interpolation
// ----------------------------------------------------------------------------

/**
 * @brief Linear interpolation between two values.
 * @param a Start value
 * @param b End value
 * @param t Interpolation parameter [0, 1]
 * @return Interpolated value
 */
template<Float T>
[[nodiscard]] constexpr T lerp(
    T const a,
    T const b,
    T const t
    ) {

    return a + (b - a) * t;
}

/**
 * @brief Linear interpolation between two vectors.
 * @param a Start vector
 * @param b End vector
 * @param t Interpolation parameter [0, 1]
 * @return Interpolated vector
 */
template <Vector2Like Vec, Numeric T>
[[nodiscard]] constexpr sf::Vector2f lerp(
    Vec const a,
    Vec const b,
    T const t
    ) {

    return {
        lerp(static_cast<float>(a.x), static_cast<float>(b.x), static_cast<float>(t)),
        lerp(static_cast<float>(a.y), static_cast<float>(b.y), static_cast<float>(t))
        };
}

/**
 * @brief Inverse linear interpolation (find t given a, b, and value).
 * @param a Start value
 * @param b End value
 * @param v Current value
 * @return Interpolation parameter that produces v
 */
template<Float T>
[[nodiscard]] constexpr T inverse_lerp(
    T const a,
    T const b,
    T const v
    ) {

    return (std::abs(b - a) < EPSILON) ? T(0) : (v - a) / (b - a);
}

/**
 * @brief Inverse linear interpolation for vectors.
 * @param a Start vector
 * @param b End vector
 * @param t Current value
 * @return Interpolation parameters for each component
 */
template <Vector2Like Vec, Numeric T>
[[nodiscard]] constexpr sf::Vector2f inverse_lerp(
    Vec const a,
    Vec const b,
    T const t
    ) {

    return {
        inverse_lerp(static_cast<float>(a.x), static_cast<float>(b.x), static_cast<float>(t)),
        inverse_lerp(static_cast<float>(a.y), static_cast<float>(b.y), static_cast<float>(t))
        };
}

/**
 * @brief Remap a value from one range to another.
 * @param v Value to remap
 * @param in_min Input range minimum
 * @param in_max Input range maximum
 * @param out_min Output range minimum
 * @param out_max Output range maximum
 * @return Remapped value
 */
template<Float T>
[[nodiscard]] constexpr T remap(
    T const v,
    T const in_min,
    T const in_max,
    T const out_min,
    T const out_max
    ) {

    return lerp(out_min, out_max, inverse_lerp(in_min, in_max, v));
}

/**
 * @brief Smooth interpolation using Hermite polynomial.
 * @param a Start value
 * @param b End value
 * @param t Interpolation parameter
 * @return Smoothly interpolated value
 */
template<Float T>
[[nodiscard]] constexpr T smoothstep(
    T const a,
    T const b,
    T t
    ) {

    t = clamp(inverse_lerp(a, b, t), T(0), T(1));

    return t * t * (T(3) - T(2) * t);
}

// ============================================================================
// Comparison
// ----------------------------------------------------------------------------

/**
 * @brief Check if two floating point values are approximately equal.
 * @param a First value
 * @param b Second value
 * @param epsilon Tolerance for comparison
 * @return True if values are within epsilon of each other
 */
template<Float T>
[[nodiscard]] constexpr bool approximately_equal(
    T const a,
    T const b,
    T const epsilon = EPSILON
    ) {

    return std::abs(a - b) <= epsilon;
}

/**
 * @brief Check if two vectors are approximately equal.
 * @param a First vector
 * @param b Second vector
 * @param epsilon Tolerance for comparison
 * @return True if all components are within epsilon
 */
template<Vector2Like Vec, Numeric T>
[[nodiscard]] constexpr bool approximately_equal(
    Vec const a,
    Vec const b,
    T const epsilon = EPSILON
    ) {

    return approximately_equal(static_cast<float>(a.x), static_cast<float>(b.x), static_cast<float>(epsilon)) &&
           approximately_equal(static_cast<float>(a.y), static_cast<float>(b.y), static_cast<float>(epsilon));
}

// ============================================================================
// Movement
// ----------------------------------------------------------------------------

/**
 * @brief Move a value towards a target by a maximum delta.
 * @param current Current value
 * @param target Target value
 * @param max_delta Maximum amount to move
 * @return New value moved towards target
 */
template<Float T>
[[nodiscard]] constexpr T move_toward(
    T const current,
    T const target,
    T const max_delta
    ) {

    if (std::abs(target - current) <= max_delta) { return target; }

    return current + std::copysign(max_delta, target - current);
}

/**
 * @brief Move a vector towards a target by a maximum distance.
 * @param current Current position
 * @param target Target position
 * @param max_delta Maximum distance to move
 * @return New position moved towards target
 */
template<Vector2Like Vec, Numeric T>
[[nodiscard]] constexpr sf::Vector2f move_toward(
    Vec const current,
    Vec const target,
    T const max_delta
    ) {

    Vec const diff = target - current;
    float const dist = length(diff);

    if (dist <= static_cast<float>(max_delta) || dist < EPSILON) { return target; }

    return current + diff * (static_cast<float>(max_delta) / dist);
}

// ============================================================================
// Angle utilities
// ----------------------------------------------------------------------------

/**
 * @brief Convert degrees to radians.
 * @param degrees Angle in degrees
 * @return Angle in radians
 */
template<Numeric T>
[[nodiscard]] constexpr T deg_to_rad(
    T const degrees
    ) {

    return static_cast<float>(degrees) * DEG2RAD;
}

/**
 * @brief Convert radians to degrees.
 * @param radians Angle in radians
 * @return Angle in degrees
 */
template<Numeric T>
[[nodiscard]] constexpr T rad_to_deg(
    T const radians
    ) {

    return static_cast<float>(radians) * RAD2DEG;
}

// ============================================================================
// Simple utilities
// ----------------------------------------------------------------------------

/**
 * @brief Get the sign of a value.
 * @param value Input value
 * @return -1 if negative, 0 if zero, 1 if positive
 */
template<Numeric T>
[[nodiscard]] constexpr int sign(
    T const value
    ) {

    return (T(0) < value) - (value < T(0));
}

/**
 * @brief Square a value.
 * @param value Input value
 * @return Value squared
 */
template<Numeric T>
[[nodiscard]] constexpr T square(
    T const value
    ) {

    return value * value;
}

// ============================================================================
// Contains
// ----------------------------------------------------------------------------

/**
 * @brief Check if a rectangle contains a point.
 * @param r Rectangle
 * @param p Point to test
 * @return True if point is inside rectangle
 */
template<RectLike Rec, Vector2Like Vec>
[[nodiscard]] constexpr bool contains(
    Rec const r,
    Vec const p
    ) {

    auto const left = r.position.x;
    auto const top = r.position.y;
    auto const right = r.position.x + r.size.x;
    auto const bottom = r.position.y + r.size.y;

    return p.x >= left && p.x <= right &&
           p.y >= top && p.y <= bottom;
}

// ============================================================================
// Intersects
// ----------------------------------------------------------------------------

/**
 * @brief Check if two rectangles intersect.
 * @param a First rectangle
 * @param b Second rectangle
 * @return True if rectangles overlap
 */
template<RectLike Rec>
[[nodiscard]] constexpr bool intersects(
    Rec const a,
    Rec const b
    ) {

    return a.position.x < b.position.x + b.size.x
        && a.position.x + a.size.x > b.position.x
        && a.position.y < b.position.y + b.size.y
        && a.position.y + a.size.y > b.position.y;
}

// ============================================================================
// Expand and shrink
// ----------------------------------------------------------------------------

/**
 * @brief Expand a rectangle by a given amount in all directions.
 * @param r Rectangle to expand
 * @param amount Amount to expand (negative to shrink)
 * @return Expanded rectangle
 */
template<RectLike Rec, Numeric T>
[[nodiscard]] constexpr auto expand(
    Rec const r,
    T const amount
    ) {

    using PosType = decltype(r.position.x);
    using SizeType = decltype(r.size.x);

    return Rec{
        static_cast<PosType>(r.position.x - amount),
        static_cast<PosType>(r.position.y - amount),
        static_cast<SizeType>(r.size.x + 2 * amount),
        static_cast<SizeType>(r.size.y + 2 * amount)
        };
}

/**
 * @brief Shrink a rectangle by a given amount in all directions.
 * @param r Rectangle to shrink
 * @param amount Amount to shrink
 * @return Shrunk rectangle
 */
template<RectLike Rec, Numeric T>
[[nodiscard]] constexpr auto shrink(
    Rec const r,
    T const amount
    ) {

    return expand(r, -amount);
}

// ============================================================================
// Center
// ----------------------------------------------------------------------------

/**
 * @brief Get the center point of a rectangle.
 * @param r Rectangle
 * @return Center point
 */
template<RectLike Rec>
[[nodiscard]] constexpr sf::Vector2f center_of(
    Rec const r
    ) {

    return {
        static_cast<float>(r.position.x) + static_cast<float>(r.size.x) * 0.5f,
        static_cast<float>(r.position.y) + static_cast<float>(r.size.y) * 0.5f
        };
}

// ============================================================================
// Area and perimeter
// ----------------------------------------------------------------------------

/**
 * @brief Calculate the area of a rectangle.
 * @param r Rectangle
 * @return Area (width * height)
 */
template<RectLike Rec>
[[nodiscard]] constexpr auto area(
    Rec const r
    ) {

    return r.size.x * r.size.y;
}

/**
 * @brief Calculate the perimeter of a rectangle.
 * @param r Rectangle
 * @return Perimeter (2 * width + 2 * height)
 */
template<RectLike Rec>
[[nodiscard]] constexpr auto perimeter(
    Rec const r
    ) {

    return 2 * (r.size.x + r.size.y);
}

// ============================================================================
// Union and intersection
// ----------------------------------------------------------------------------

/**
 * @brief Calculate the bounding box that contains both rectangles.
 * @param a First rectangle
 * @param b Second rectangle
 * @return Union rectangle
 */
template<RectLike Rec>
[[nodiscard]] constexpr Rec union_of(
    Rec const a,
    Rec const b
    ) {

    auto const min_x = std::min(a.position.x, b.position.x);
    auto const min_y = std::min(a.position.y, b.position.y);
    auto const max_x = std::max(a.position.x + a.size.x, b.position.x + b.size.x);
    auto const max_y = std::max(a.position.y + a.size.y, b.position.y + b.size.y);

    return {
        min_x,
        min_y,
        max_x - min_x,
        max_y - min_y
        };
}

/**
 * @brief Calculate the overlapping region of two rectangles.
 * @param a First rectangle
 * @param b Second rectangle
 * @return Intersection rectangle, or nullopt if no overlap
 */
template<RectLike Rec>
[[nodiscard]] constexpr std::optional<Rec> intersection_of(
    Rec const a,
    Rec const b
    ) {

    auto const min_x = std::max(a.position.x, b.position.x);
    auto const min_y = std::max(a.position.y, b.position.y);
    auto const max_x = std::min(a.position.x + a.size.x, b.position.x + b.size.x);
    auto const max_y = std::min(a.position.y + a.size.y, b.position.y + b.size.y);

    if (max_x > min_x && max_y > min_y) {

        return Rec {
            min_x,
            min_y,
            max_x - min_x,
            max_y - min_y
            };
    }

    return std::nullopt;
}

} // namespace titan::utils
