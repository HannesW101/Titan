// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-utils/include/random.hpp"
#include "module-utils/include/math.hpp"

#include <chrono>
#include <limits>
#include <bit>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::utils {

// ============================================================================
// Helper functions
// ----------------------------------------------------------------------------

namespace {

// ----------------------------------------------------------------------------
[[nodiscard]] constexpr std::uint64_t splitmix64(
    std::uint64_t& state
    ) {

    std::uint64_t z = (state += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

// ----------------------------------------------------------------------------
[[nodiscard]] constexpr std::uint64_t rotl(
    std::uint64_t const x,
    int const k
    ) {

    return (x << k) | (x >> (64 - k));
}

}

// ============================================================================
// Class Random
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Random::Random()
    : Random(static_cast<std::uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
        ))
{}

// ----------------------------------------------------------------------------
Random::Random(
    std::uint64_t const seed
    )
    : _state{}
    , _seed(seed) {

    std::uint64_t s = seed;
    _state[0u] = splitmix64(s);
    _state[1u] = splitmix64(s);
    _state[2u] = splitmix64(s);
    _state[3u] = splitmix64(s);
}

// ----------------------------------------------------------------------------
void Random::seed(
    std::uint64_t const s
    ) {

    _seed = s;

    std::uint64_t temp = s;
    _state[0u] = splitmix64(temp);
    _state[1u] = splitmix64(temp);
    _state[2u] = splitmix64(temp);
    _state[3u] = splitmix64(temp);
}

// ----------------------------------------------------------------------------
std::uint64_t Random::current_seed() const {

    return _seed;
}

// ----------------------------------------------------------------------------
std::uint64_t Random::_next() {

    std::uint64_t const result = rotl(_state[0u] + _state[3u], 23u) + _state[0u];
    std::uint64_t const t = _state[1u] << 17u;

    _state[2u] ^= _state[0u];
    _state[3u] ^= _state[1u];
    _state[1u] ^= _state[2u];
    _state[0u] ^= _state[3u];

    _state[2u] ^= t;
    _state[3u] = rotl(_state[3], 45);

    return result;
}

// ----------------------------------------------------------------------------
int Random::range(
    int const min,
    int const max
    ) {

    if (min > max) { return range(max, min); }
    if (min == max) { return min; }

    std::uint32_t const range_size = static_cast<std::uint32_t>(max - min) + 1;
    std::uint64_t random = static_cast<std::uint32_t>(_next());
    std::uint64_t product = random * range_size;
    std::uint32_t low = static_cast<std::uint32_t>(product);

    if (low < range_size) {

        std::uint32_t const threshold = -range_size % range_size;

        while (low < threshold) {

            random = static_cast<std::uint32_t>(_next());
            product = random * range_size;
            low = static_cast<std::uint32_t>(product);
        }
    }

    return min + static_cast<int>(product >> 32);
}

// ----------------------------------------------------------------------------
long Random::range(
    long const min,
    long const max
    ) {

    if (min > max) { return range(max, min); }
    if (min == max) { return min; }

    std::uint64_t const range_size = static_cast<std::uint64_t>(max - min) + 1;
    std::uint64_t const limit = (std::numeric_limits<std::uint64_t>::max() / range_size) * range_size;

    std::uint64_t value = 0;

    do {

        value = _next();
    } while (value >= limit);

    return min + static_cast<long>(value % range_size);
}

// ----------------------------------------------------------------------------
float Random::range(
    float const min,
    float const max
    ) {

    std::uint32_t const bits = static_cast<std::uint32_t>(_next() >> 40);
    float const unit = static_cast<float>(bits) * (1.0f / 16777216.0f);

    return lerp(min, max, unit);
}

// ----------------------------------------------------------------------------
double Random::range(
    double const min,
    double const max
    ) {

    std::uint64_t const bits = _next() >> 11;
    double const unit = static_cast<double>(bits) * (1.0 / 9007199254740992.0);

    return lerp(min, max, unit);
}

// ----------------------------------------------------------------------------
bool Random::chance(
    float const probability
    ) {

    return range(0.0f, 1.0f) < probability;
}

// ----------------------------------------------------------------------------
sf::Vector2f Random::in_unit_circle() {

    float x = 0.0f;
    float y = 0.0f;
    float len_sq = 0.0f;

    do {

        x      = range(-1.0f, 1.0f);
        y      = range(-1.0f, 1.0f);
        len_sq = length_sq(sf::Vector2f{x, y});
    } while (len_sq > 1.0f || len_sq < EPSILON);

    return {x, y};
}

// ----------------------------------------------------------------------------
sf::Vector2f Random::in_circle(
    float const radius
    ) {

    return in_unit_circle() * radius;
}

// ----------------------------------------------------------------------------
sf::Vector2f Random::on_unit_circle() {

    float const angle = range(0.0f, TAU);

    return from_angle(angle);
}

// ----------------------------------------------------------------------------
sf::Vector2f Random::in_rect(
    sf::FloatRect const rect
    ) {

    return {
        range(rect.position.x, rect.position.x + rect.size.x),
        range(rect.position.y, rect.position.y + rect.size.y)
        };
}

// ----------------------------------------------------------------------------
Random& Random::global() {

    static Random instance;
    return instance;
}

} // namespace titan::utils
