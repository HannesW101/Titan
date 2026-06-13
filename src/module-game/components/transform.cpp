// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/components/include/transform.hpp"

#include "module-utils/include/math.hpp"

#include "SFML/System/Angle.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Using directives
// ----------------------------------------------------------------------------

using namespace titan::utils;

// ============================================================================
// Class Transform
// ----------------------------------------------------------------------------

// ---- Local space -----------------------------------------------------------

// ----------------------------------------------------------------------------
sf::Vector2f Transform::position    (                            ) const { return _position;     }
void         Transform::set_position(sf::Vector2f const p        )       { _position = p;        }
void         Transform::set_position(float const x, float const y)       { _position = { x, y }; }
void         Transform::move        (sf::Vector2f const delta    )       { _position += delta;   }

// ----------------------------------------------------------------------------
float Transform::rotation    (                   ) const { return _rotation;     }
void  Transform::set_rotation(float const degrees)       { _rotation = degrees;  }
void  Transform::rotate      (float const degrees)       { _rotation += degrees; }

// ----------------------------------------------------------------------------
sf::Vector2f Transform::scale    (                            ) const { return _scale;     }
void         Transform::set_scale(sf::Vector2f const s        )       { _scale = s;        }
void         Transform::set_scale(float const x, float const y)       { _scale = { x, y }; }

// ---- World space -----------------------------------------------------------

// ----------------------------------------------------------------------------
sf::Vector2f Transform::world_position() const {

    if (!_parent) { return _position; }

    // Compose: parent world matrix applied to our local position.
    return _parent->world_matrix().transformPoint(_position);
}

// ----------------------------------------------------------------------------
float Transform::world_rotation() const {

    return _parent ? _parent->world_rotation() + _rotation : _rotation;
}

// ----------------------------------------------------------------------------
sf::Vector2f Transform::world_scale() const {

    if (!_parent) { return _scale; }

    sf::Vector2f const ps = _parent->world_scale();

    return { ps.x * _scale.x, ps.y * _scale.y };
}

// ----------------------------------------------------------------------------
sf::Transform Transform::world_matrix() const {

    sf::Transform t;
    t.translate(_position);
    t.rotate(sf::degrees(_rotation));
    t.scale(_scale);

    if (_parent) { return _parent->world_matrix() * t; }

    return t;
}

// ---- Parenting -------------------------------------------------------------

// ----------------------------------------------------------------------------
void       Transform::set_parent(Transform* parent)       { _parent = parent; }
Transform* Transform::parent    (                 ) const { return _parent;   }

} // namespace titan::game
