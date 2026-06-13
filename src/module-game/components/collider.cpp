// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/collider.hpp"
#include "module-game/include/game_object.hpp"
#include "module-game/components/include/transform.hpp"

#include "module-utils/include/math.hpp"

#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Collider
// ----------------------------------------------------------------------------

// ---- Shape -----------------------------------------------------------------

// ----------------------------------------------------------------------------
void Collider::set_box(
    sf::Vector2f const half_extents
    ) {

    _shape        = Collider_shape::AABB;
    _half_extents = half_extents;
}

// ----------------------------------------------------------------------------
void Collider::set_circle(
    float const radius
    ) {

    _shape  = Collider_shape::CIRCLE;
    _radius = radius;
}

// ----------------------------------------------------------------------------
Collider_shape Collider::shape       () const { return _shape;        }
sf::Vector2f   Collider::half_extents() const { return _half_extents; }
float          Collider::radius      () const { return _radius;       }

// ----------------------------------------------------------------------------
void         Collider::set_offset(sf::Vector2f const offset)       { _offset = offset; }
sf::Vector2f Collider::offset    (                         ) const { return _offset;   }

// ----------------------------------------------------------------------------
void Collider::set_trigger(bool const trigger)       { _trigger = trigger; }
bool Collider::is_trigger (                  ) const { return _trigger;    }

// ---- World-space helpers ---------------------------------------------------

// ----------------------------------------------------------------------------
sf::Vector2f Collider::world_center() const {

    return owner().transform().world_position() + _offset;
}

// ----------------------------------------------------------------------------
sf::FloatRect Collider::world_aabb() const {

    sf::Vector2f const c = world_center();

    if (_shape == Collider_shape::CIRCLE) {

        return sf::FloatRect(
            { c.x - _radius, c.y - _radius },
            { _radius * 2.0f, _radius * 2.0f }
            );
    }

    return sf::FloatRect(
        { c.x - _half_extents.x, c.y - _half_extents.y },
        { _half_extents.x * 2.0f, _half_extents.y * 2.0f }
        );
}

// ----------------------------------------------------------------------------
bool Collider::overlaps(
    Collider const& other
    ) const {

    // Circle vs circle: compare centre distance to summed radii (math lib).
    if (_shape == Collider_shape::CIRCLE && other._shape == Collider_shape::CIRCLE) {

        float const d = distance(world_center(), other.world_center());
        return d <= (_radius + other._radius);
    }

    // Any case involving a box: use AABB intersection (math lib). For circles
    // this uses their bounding box, an acceptable broad-phase-style test here.
    return intersects(world_aabb(), other.world_aabb());
}

// ---- Callbacks -------------------------------------------------------------

// ----------------------------------------------------------------------------
void Collider::set_on_enter(Collision_fn fn) { _on_enter = std::move(fn); }
void Collider::set_on_stay (Collision_fn fn) { _on_stay  = std::move(fn); }
void Collider::set_on_exit (Collision_fn fn) { _on_exit  = std::move(fn); }

// ----------------------------------------------------------------------------
void Collider::fire_enter(Collider& other) { if (_on_enter) { _on_enter(*this, other); } }
void Collider::fire_stay (Collider& other) { if (_on_stay)  { _on_stay(*this, other);  } }
void Collider::fire_exit (Collider& other) { if (_on_exit)  { _on_exit(*this, other);  } }

} // namespace titan::game
