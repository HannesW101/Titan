#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_object.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::render { class Renderer; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class World
// ----------------------------------------------------------------------------

/**
 * @brief Container and driver for all Game_objects in a scene.
 *
 * Spawns objects (assigning unique ids), forwards the engine loop to them, and
 * reaps destroyed objects at a safe point so components can destroy objects
 * (including their own) mid-update without invalidating iteration.
 *
 * A Scene typically owns one World:
 *
 *   class Gameplay : public scene::Scene {
 *       game::World _world;
 *       void fixed_update(float dt) override { _world.fixed_update(dt); }
 *       void update(float dt)       override { _world.update(dt); }
 *       void render(render::Renderer& r) override { _world.render(r); }
 *   };
 */
class World final {

public:
    /**
     * @brief Construct an empty world.
     */
    World() = default;

    World           (World const&) = delete;
    World& operator=(World const&) = delete;
    World           (World&&     ) = delete;
    World& operator=(World&&     ) = delete;

    // ---- Spawning ----------------------------------------------------------

    /**
     * @brief Create a new Game_object in the world.
     * @param name Optional name (defaults to "object")
     * @return Non-owning pointer to the new object (owned by the World)
     */
    Game_object* spawn(std::string name = "object");

    /**
     * @brief Immediately destroy and remove all objects.
     */
    void clear();

    // ---- Queries -----------------------------------------------------------

    /**
     * @brief Find the first live object with the given name.
     * @param name Name to search for
     * @return Pointer to the object, or nullptr if none match
     */
    Game_object* find(std::string const& name);

    /**
     * @brief Find the live object with the given id.
     * @param id Unique identifier to search for
     * @return Pointer to the object, or nullptr if not found
     */
    Game_object* find_by_id(std::uint64_t const id);

    /**
     * @brief Get the number of objects (live plus pending-spawn).
     * @return Object count
     */
    std::size_t count() const;

    /**
     * @brief Access all live objects (for systems that iterate).
     * @return Reference to the object vector
     */
    std::vector<std::unique_ptr<Game_object>>& objects();

    /**
     * @brief Access all live objects (const).
     * @return Const reference to the object vector
     */
    std::vector<std::unique_ptr<Game_object>> const& objects() const;

    // ---- Per-frame ---------------------------------------------------------

    /**
     * @brief Fixed-timestep update; forwards to all objects.
     * @param fixed_dt Fixed delta time in seconds
     */
    void fixed_update(float const fixed_dt);

    /**
     * @brief Per-frame update; merges spawns, updates objects, reaps destroyed.
     * @param dt Frame delta time in seconds
     */
    void update(float const dt);

    /**
     * @brief Per-frame render; forwards to all objects.
     * @param renderer Renderer to submit drawables to
     */
    void render(render::Renderer& renderer);

private:
    /**
     * @brief Remove objects marked for destruction.
     */
    void _reap_destroyed();

    std::vector<std::unique_ptr<Game_object>> _objects; ///< Live objects
    std::vector<std::unique_ptr<Game_object>> _spawned_this_frame; ///< Pending merge
    std::uint64_t                             _next_id = 1u; ///< Next object id
};

} // namespace titan::game
