#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Transform.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Transform
// ----------------------------------------------------------------------------

/**
 * @brief Position / rotation / scale of a Game_object in world space, with
 * optional parenting for hierarchical transforms.
 *
 * Every Game_object owns a Transform implicitly (created in its constructor),
 * so you rarely add one manually. Other components read it via
 * owner().transform().
 *
 * Local vs world: if the object has a transform parent, local_* values are
 * relative to the parent and world_* compose up the chain. With no parent,
 * local and world are identical.
 */
class Transform final : public Game_component {

public:
    /**
     * @brief Construct an identity transform (origin, no rotation, unit scale).
     */
    Transform() = default;

    // ---- Local space -------------------------------------------------------

    /**
     * @brief Get the local position.
     * @return Local position in pixels
     */
    sf::Vector2f position() const;

    /**
     * @brief Set the local position.
     * @param p New local position
     */
    void set_position(sf::Vector2f const p);

    /**
     * @brief Set the local position from components.
     * @param x X coordinate
     * @param y Y coordinate
     */
    void set_position(float const x, float const y);

    /**
     * @brief Translate the local position by a delta.
     * @param delta Offset to add to the current position
     */
    void move(sf::Vector2f const delta);

    /**
     * @brief Get the local rotation.
     * @return Rotation in degrees
     */
    float rotation() const;

    /**
     * @brief Set the local rotation.
     * @param degrees New rotation in degrees
     */
    void set_rotation(float const degrees);

    /**
     * @brief Rotate the local rotation by a delta.
     * @param degrees Degrees to add to the current rotation
     */
    void rotate(float const degrees);

    /**
     * @brief Get the local scale.
     * @return Scale factors per axis
     */
    sf::Vector2f scale() const;

    /**
     * @brief Set the local scale.
     * @param s New scale factors per axis
     */
    void set_scale(sf::Vector2f const s);

    /**
     * @brief Set the local scale from components.
     * @param x X scale factor
     * @param y Y scale factor
     */
    void set_scale(float const x, float const y);

    // ---- World space (composes parent chain) -------------------------------

    /**
     * @brief Get the world position, composing the parent chain.
     * @return World position in pixels
     */
    sf::Vector2f world_position() const;

    /**
     * @brief Get the world rotation, composing the parent chain.
     * @return World rotation in degrees
     */
    float world_rotation() const;

    /**
     * @brief Get the world scale, composing the parent chain.
     * @return World scale factors per axis
     */
    sf::Vector2f world_scale() const;

    /**
     * @brief Get the combined world transform matrix.
     * @return SFML transform suitable for RenderStates or point transforms
     */
    sf::Transform world_matrix() const;

    // ---- Parenting ---------------------------------------------------------

    /**
     * @brief Set the parent transform (nullptr to detach).
     * @param parent Parent transform, or nullptr for none
     */
    void set_parent(Transform* parent);

    /**
     * @brief Get the parent transform.
     * @return Parent transform, or nullptr if none
     */
    Transform* parent() const;

private:
    sf::Vector2f _position = { 0.0f, 0.0f }; ///< Local position
    float        _rotation = 0.0f; ///< Local rotation (degrees)
    sf::Vector2f _scale    = { 1.0f, 1.0f }; ///< Local scale

    Transform* _parent = nullptr; ///< Parent transform, or null
};

} // namespace titan::game
