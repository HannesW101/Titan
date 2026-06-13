#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include <unordered_set>
#include <vector>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

class World;
class Collider;

// ============================================================================
// Class Collision_system
// ----------------------------------------------------------------------------

/**
 * @brief Detects overlaps between all active Colliders in a World and fires
 * enter / stay / exit callbacks.
 *
 * Call update(world) once per fixed step (after movement). It gathers every
 * enabled Collider on active, non-destroyed objects, tests each unique pair,
 * and tracks which pairs were touching last step so it can distinguish enter
 * (new), stay (still), and exit (no longer) and fire the matching callbacks on
 * both colliders.
 *
 *   collision_system.update(world);   // in Scene::fixed_update after world step
 *
 * The current broad test is O(n^2) pairwise, which is fine for the hundreds of
 * colliders a typical 2D game has, a spatial grid can replace _broad_phase
 * later without changing the callback contract.
 */
class Collision_system final {

public:
    /**
     * @brief Construct a collision system with no tracked contacts.
     */
    Collision_system() = default;

    /**
     * @brief Test all collider pairs and fire enter/stay/exit callbacks.
     * @param world World whose colliders to test
     */
    void update(World& world);

    /**
     * @brief Forget all tracked contacts (e.g. on scene reset).
     */
    void clear();

private:
    /**
     * @brief Encode an unordered pair of object ids into one key.
     * @param a First object id
     * @param b Second object id
     * @return Order-independent pair key
     */
    static std::uint64_t _pair_key(std::uint64_t a, std::uint64_t b);

    /**
     * @brief Gather enabled colliders on active, non-destroyed objects.
     * @param world World to scan
     * @return Vector of collider pointers
     */
    std::vector<Collider*> _broad_phase(World& world) const;

    std::unordered_set<std::uint64_t> _previous_contacts; ///< Pairs touching last step
};

} // namespace titan::game
