#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include "SFML/System/Vector2.hpp"

namespace titan::game {

// ============================================================================
// Class Rigidbody
// ----------------------------------------------------------------------------

/**
 * @brief Simple kinematic motion: velocity, acceleration, forces, drag and
 * gravity, integrated into the owner's Transform each fixed step.
 *
 * This is deliberately lightweight (not a full physics engine). It moves the
 * Transform by integrating velocity semi-implicitly, which is stable for
 * top-down movement, platformer-ish gravity, projectiles, knockback, etc.
 * Pair with a Collider + Collision_system for response logic in callbacks.
 *
 *   auto* rb = obj->add_component<Rigidbody>();
 *   rb->set_gravity({ 0.f, 980.f });
 *   rb->add_force({ 200.f, 0.f });          // one-shot impulse-ish push
 *   rb->set_velocity({ 50.f, 0.f });
 */
class Rigidbody final : public Game_component {

public:
    /**
     * @brief Construct a rigidbody at rest with unit mass and no gravity.
     */
    Rigidbody() = default;

    /**
     * @brief Set the velocity.
     * @param v Velocity in pixels/second
     */
    void set_velocity(sf::Vector2f const v);

    /**
     * @brief Get the velocity.
     * @return Velocity in pixels/second
     */
    sf::Vector2f velocity() const;

    /**
     * @brief Set the constant base acceleration.
     * @param a Acceleration in pixels/second^2
     */
    void set_acceleration(sf::Vector2f const a);

    /**
     * @brief Get the constant base acceleration.
     * @return Acceleration in pixels/second^2
     */
    sf::Vector2f acceleration() const;

    /**
     * @brief Accumulate a force for the next fixed step (scaled by 1/mass).
     * @param force Force vector; cleared after the next integration
     */
    void add_force(sf::Vector2f const force);

    /**
     * @brief Set the mass (clamped to a small positive minimum).
     * @param mass Mass value
     */
    void set_mass(float const mass);

    /**
     * @brief Get the mass.
     * @return Mass value
     */
    float mass() const;

    /**
     * @brief Set the gravity acceleration.
     * @param g Gravity in pixels/second^2
     */
    void set_gravity(sf::Vector2f const g);

    /**
     * @brief Get the gravity acceleration.
     * @return Gravity in pixels/second^2
     */
    sf::Vector2f gravity() const;

    /**
     * @brief Set linear damping in [0,1): fraction of velocity lost per second.
     * @param damping Damping factor (0 = none)
     */
    void set_linear_damping(float const damping);

    /**
     * @brief Get the linear damping factor.
     * @return Damping factor
     */
    float linear_damping() const;

    /**
     * @brief Integrate motion and move the owner's Transform.
     * @param fixed_dt Fixed delta time in seconds
     */
    void fixed_update(float const fixed_dt) override;

private:
    sf::Vector2f _velocity          = { 0.0f, 0.0f }; ///< Current velocity
    sf::Vector2f _acceleration      = { 0.0f, 0.0f }; ///< Constant base acceleration
    sf::Vector2f _accumulated_force = { 0.0f, 0.0f }; ///< Forces for next step
    sf::Vector2f _gravity           = { 0.0f, 0.0f }; ///< Gravity acceleration
    float        _mass              = 1.0f; ///< Mass
    float        _linear_damping    = 0.0f; ///< Linear damping factor
};

} // namespace titan::game
