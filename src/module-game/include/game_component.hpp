#pragma once

// ============================================================================
// Forward declarations (1/2)
// ----------------------------------------------------------------------------

namespace titan::render { class Renderer; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Forward declarations (2/2)
// ----------------------------------------------------------------------------

class Game_object;

// ============================================================================
// Class Game_component
// ----------------------------------------------------------------------------

/**
 * @brief Base class for all behavior/data attached to a Game_object.
 *
 * Components are the unit of composition: a Game_object is little more than a
 * named bag of components plus a Transform. Each component gets lifecycle hooks
 * mirroring the engine loop and a back-pointer to its owner.
 *
 * Lifecycle:
 *   on_attach()        once, when added to an object that's in the world
 *   on_start()         once, on the first frame the object is active
 *   fixed_update(dt)   zero+ times/frame (physics, gameplay)
 *   update(dt)         once/frame (animation, input-driven logic)
 *   render(renderer)   once/frame (visual components)
 *   on_detach()        once, when removed or the object is destroyed
 *
 * Components reach siblings through owner(): e.g. a Sprite_renderer reads the
 * Transform via owner().transform().
 */
class Game_component {

public:
    /**
     * @brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~Game_component() = default;

    Game_component           (Game_component const&) = delete;
    Game_component& operator=(Game_component const&) = delete;
    Game_component           (Game_component&&     ) = delete;
    Game_component& operator=(Game_component&&     ) = delete;

    // ---- Lifecycle hooks ---------------------------------------------------

    /**
     * @brief Called once when the component is attached to its owner.
     */
    virtual void on_attach() {}

    /**
     * @brief Called once on the first frame the owning object is active.
     */
    virtual void on_start()  {}

    /**
     * @brief Called once when the component is removed or its owner destroyed.
     */
    virtual void on_detach() {}

    /**
     * @brief Fixed-timestep update for physics/gameplay logic.
     * @param fixed_dt Fixed delta time in seconds
     */
    virtual void fixed_update(float const fixed_dt) { static_cast<void>(fixed_dt); }

    /**
     * @brief Per-frame update for animation and input-driven logic.
     * @param dt Frame delta time in seconds
     */
    virtual void update(float const dt) { static_cast<void>(dt); }

    /**
     * @brief Per-frame render hook for visual components.
     * @param renderer Renderer to submit drawables to
     */
    virtual void render(render::Renderer& renderer) { static_cast<void>(renderer); }

    // ---- Enable flag -------------------------------------------------------

    /**
     * @brief Check whether the component is enabled.
     * @return True if enabled (receives updates/renders)
     */
    bool is_enabled() const;

    /**
     * @brief Enable or disable the component.
     * @param enabled True to enable, false to skip its updates/renders
     */
    void set_enabled(bool const enabled);

    // ---- Owner -------------------------------------------------------------

    /**
     * @brief Get the owning Game_object.
     * @return Reference to the owner
     * @throws std::runtime_error if the component has no owner
     */
    Game_object& owner();

    /**
     * @brief Get the owning Game_object (const).
     * @return Const reference to the owner
     * @throws std::runtime_error if the component has no owner
     */
    Game_object const& owner() const;

    /**
     * @brief Check whether the component is attached to an owner.
     * @return True if an owner is set
     */
    bool has_owner() const;

protected:
    /**
     * @brief Default-construct a component (only derived classes).
     */
    Game_component() = default;

private:
    friend class Game_object;

    Game_object* _owner   = nullptr; ///< Owning object (set on attach)
    bool         _enabled = true; ///< Whether updates/renders run
    bool         _started = false; ///< Whether on_start has fired
};

} // namespace titan::game
