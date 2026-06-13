#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include <algorithm>
#include <cstdint>
#include <span>
#include <array>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::utils {

// ============================================================================
// Class Random
// ----------------------------------------------------------------------------

/**
 * @brief High-quality random number generator using xoshiro256++ algorithm.
 *
 * Provides cryptographically-strong pseudo-random number generation with
 * excellent statistical properties. Passes all BigCrush and PractRand tests.
 * Much faster and higher quality than std::mt19937.
 *
 * Supports deterministic seeding for reproducible sequences, making it
 * suitable for procedural generation, simulations, and gameplay.
 */
class Random final {

public:
    /**
     * @brief Construct with time-based seed.
     */
    Random();

    /**
     * @brief Construct with explicit seed for reproducible sequences.
     * @param seed Initial seed value
     */
    explicit Random(std::uint64_t const seed);

    /**
     * @brief Reseed the generator.
     * @param s New seed value
     */
    void seed(std::uint64_t const s);

    /**
     * @brief Get the current seed value.
     * @return Current seed
     */
    std::uint64_t current_seed() const;

    /**
     * @brief Generate random integer in range [min, max] (inclusive).
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return Random integer in range
     */
    int range(int const min, int const max);

    /**
     * @brief Generate random long integer in range [min, max] (inclusive).
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return Random long in range
     */
    long range(long const min, long const max);

    /**
     * @brief Generate random float in range [min, max).
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return Random float in range
     */
    float range(float const min, float const max);

    /**
     * @brief Generate random double in range [min, max).
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return Random double in range
     */
    double range(double const min, double const max);

    /**
     * @brief Return true with given probability.
     * @param probability Probability of returning true [0.0, 1.0]
     * @return True with given probability, false otherwise
     */
    bool chance(float const probability);

    /**
     * @brief Generate random point inside unit circle.
     * @return Point uniformly distributed inside circle of radius 1
     */
    sf::Vector2f in_unit_circle();

    /**
     * @brief Generate random point inside circle of given radius.
     * @param radius Radius of circle
     * @return Point uniformly distributed inside circle
     */
    sf::Vector2f in_circle(float const radius);

    /**
     * @brief Generate random point on unit circle perimeter.
     * @return Point on circle of radius 1
     */
    sf::Vector2f on_unit_circle();

    /**
     * @brief Generate random point inside rectangle.
     * @param rect Rectangle bounds
     * @return Point uniformly distributed inside rectangle
     */
    sf::Vector2f in_rect(sf::FloatRect const rect);

    /**
     * @brief Get global shared Random instance.
     * @return Reference to global Random instance
     */
    static Random& global();

public:
    /**
     * @brief Pick a random element from a collection.
     * @tparam T Element type
     * @param items Collection to pick from
     * @return Reference to randomly selected element
     */
    template<typename T>
    T& pick(std::span<T> items) {

        return items[range(0, static_cast<int>(items.size()) - 1)];
    }

    /**
     * @brief Randomly shuffle a collection using Fisher-Yates algorithm.
     * @tparam T Element type
     * @param items Collection to shuffle in-place
     */
    template<typename T>
    void shuffle(std::span<T> items) {

        for (std::size_t i = items.size() - 1; i > 0; --i) {

            std::size_t const j = range(static_cast<long>(0), static_cast<long>(i));
            std::swap(items[i], items[j]);
        }
    }

private:
    /**
     * @brief Generate next raw 64-bit random value using xoshiro256++.
     * @return Raw random bits
     */
    std::uint64_t _next();

    std::array<std::uint64_t, 4u> _state; ///< Generator state (256 bits)
    std::uint64_t                 _seed; ///< Current seed for reproducibility
};

} // namespace titan::utils
