#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include <cstdint>
#include <functional>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

class Game_object;

// ============================================================================
// Enum Collider_shape
// ----------------------------------------------------------------------------

/**
 * @brief The geometric shape a Collider tests with.
 */
enum class Collider_shape : std::uint8_t {

    AABB, ///< Axis-aligned box, sized by half_extents
    CIRCLE ///< Circle, sized by radius
};

// ============================================================================
// Class Collider
// ----------------------------------------------------------------------------

/**
 * @brief A collision shape attached to a Game_object, positioned at the owner's
 * world position plus a local offset.
 *
 * Two shapes are supported: AABB (set via half-extents) and circle (radius).
 * The Collision_system tests every enabled collider pair each fixed step and
 * fires on_enter / on_stay / on_exit callbacks. A collider may be a trigger
 * (overlap reported but no implied physical response).
 *
 *   auto* c = obj->add_component<Collider>();
 *   c->set_box({ 16.f, 16.f });          // 32x32 box
 *   c->set_on_enter([](Collider& self, Collider& other){ ... });
 */
class Collider final : public Game_component {

public:
    using Collision_fn = std::function<void(Collider& self, Collider& other)>; ///< Collision callback

    /**
     * @brief Construct a default 32x32 AABB collider.
     */
    Collider() = default;

    // ---- Shape -------------------------------------------------------------

    /**
     * @brief Configure as an axis-aligned box.
     * @param half_extents Half width/height (full box is twice this)
     */
    void set_box(sf::Vector2f const half_extents);

    /**
     * @brief Configure as a circle.
     * @param radius Circle radius
     */
    void set_circle(float const radius);

    /**
     * @brief Get the collider shape.
     * @return Current shape enum
     */
    Collider_shape shape() const;

    /**
     * @brief Get the box half-extents.
     * @return Half width/height
     */
    sf::Vector2f half_extents() const;

    /**
     * @brief Get the circle radius.
     * @return Radius
     */
    float radius() const;

    /**
     * @brief Set the local offset from the owner's position.
     * @param offset Offset in pixels
     */
    void set_offset(sf::Vector2f const offset);

    /**
     * @brief Get the local offset.
     * @return Offset in pixels
     */
    sf::Vector2f offset() const;

    /**
     * @brief Mark the collider as a trigger (overlap-only, no response).
     * @param trigger True for a trigger
     */
    void set_trigger(bool const trigger);

    /**
     * @brief Check whether the collider is a trigger.
     * @return True if a trigger
     */
    bool is_trigger() const;

    // ---- World-space helpers (used by Collision_system) --------------------

    /**
     * @brief Get the world-space center (owner position plus offset).
     * @return World center in pixels
     */
    sf::Vector2f world_center() const;

    /**
     * @brief Get the world-space bounding box (also for circles).
     * @return Bounding rectangle
     */
    sf::FloatRect world_aabb() const;

    /**
     * @brief Test overlap against another collider.
     * @param other Other collider
     * @return True if the shapes overlap
     */
    bool overlaps(Collider const& other) const;

    // ---- Callbacks ---------------------------------------------------------

    /**
     * @brief Set the callback fired when a new overlap begins.
     * @param fn Callback receiving self and other
     */
    void set_on_enter(Collision_fn fn);

    /**
     * @brief Set the callback fired while an overlap continues.
     * @param fn Callback receiving self and other
     */
    void set_on_stay(Collision_fn fn);

    /**
     * @brief Set the callback fired when an overlap ends.
     * @param fn Callback receiving self and other
     */
    void set_on_exit(Collision_fn fn);

    /**
     * @brief Fire the enter callback (called by Collision_system).
     * @param other The other collider
     */
    void fire_enter(Collider& other);

    /**
     * @brief Fire the stay callback (called by Collision_system).
     * @param other The other collider
     */
    void fire_stay(Collider& other);

    /**
     * @brief Fire the exit callback (called by Collision_system).
     * @param other The other collider
     */
    void fire_exit(Collider& other);

private:
    Collider_shape _shape        = Collider_shape::AABB; ///< Active shape
    sf::Vector2f   _half_extents = { 16.0f, 16.0f }; ///< Box half-extents
    float          _radius       = 16.0f; ///< Circle radius
    sf::Vector2f   _offset       = { 0.0f, 0.0f }; ///< Local offset
    bool           _trigger      = false; ///< Trigger flag

    Collision_fn _on_enter; ///< Enter callback
    Collision_fn _on_stay; ///< Stay callback
    Collision_fn _on_exit; ///< Exit callback
};

} // namespace titan::game
