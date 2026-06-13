// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/rigidbody.hpp"
#include "module-game/include/game_object.hpp"
#include "module-game/components/include/transform.hpp"

#include "module-utils/include/math.hpp"

#include <algorithm>
#include <cmath>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Rigidbody
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void         Rigidbody::set_velocity(sf::Vector2f const v)       { _velocity = v;    }
sf::Vector2f Rigidbody::velocity    (                    ) const { return _velocity; }

// ----------------------------------------------------------------------------
void         Rigidbody::set_acceleration(sf::Vector2f const a)       { _acceleration = a;    }
sf::Vector2f Rigidbody::acceleration    (                    ) const { return _acceleration; }

// ----------------------------------------------------------------------------
void Rigidbody::add_force(sf::Vector2f const force) { _accumulated_force += force; }

// ----------------------------------------------------------------------------
void  Rigidbody::set_mass(float const mass)       { _mass = std::max(0.0001f, mass); }
float Rigidbody::mass    (                ) const { return _mass;                    }

// ----------------------------------------------------------------------------
void         Rigidbody::set_gravity(sf::Vector2f const g)       { _gravity = g;    }
sf::Vector2f Rigidbody::gravity    (                    ) const { return _gravity; }

// ----------------------------------------------------------------------------
void  Rigidbody::set_linear_damping(float const d)       { _linear_damping = clamp01(d); }
float Rigidbody::linear_damping    (             ) const { return _linear_damping;       }

// ----------------------------------------------------------------------------
void Rigidbody::fixed_update(
    float const fixed_dt
    ) {

    // a = base acceleration + gravity + F/m
    sf::Vector2f const accel = _acceleration + _gravity + (_accumulated_force / _mass);

    // Semi-implicit Euler: update velocity, then position.
    _velocity += accel * fixed_dt;

    // Frame-rate independent damping: retain (1 - damping)^dt of velocity.
    if (_linear_damping > 0.0f) {

        float const retain = std::pow(1.0f - _linear_damping, fixed_dt);
        _velocity *= retain;
    }

    owner().transform().move(_velocity * fixed_dt);

    _accumulated_force = { 0.0f, 0.0f };
}

} // namespace titan::game
