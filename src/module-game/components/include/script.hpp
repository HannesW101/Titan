#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include <functional>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Script
// ----------------------------------------------------------------------------

/**
 * @brief A component that runs user-provided callbacks for the lifecycle hooks,
 * so simple behaviors don't need a whole new component subclass.
 *
 *   auto* s = obj->add_component<Script>();
 *   s->set_start_fn ([](Game_object& self){ ... });
 *   s->set_update_fn([](Game_object& self, float dt){
 *       self.transform().move({ 10.f * dt, 0.f });
 *   });
 *
 * Each callback receives the owning Game_object by reference, so behavior can
 * reach the transform, siblings, and the world.
 */
class Script final : public Game_component {

public:
    using Start_fn        = std::function<void(Game_object&)>; ///< Start callback
    using Update_fn       = std::function<void(Game_object&, float)>; ///< Update callback
    using Fixed_update_fn = std::function<void(Game_object&, float)>; ///< Fixed-update callback
    using Detach_fn       = std::function<void(Game_object&)>; ///< Detach callback

    /**
     * @brief Construct a script with no callbacks set.
     */
    Script() = default;

    /**
     * @brief Set the callback run once on start.
     * @param fn Callback receiving the owner
     */
    void set_start_fn(Start_fn fn);

    /**
     * @brief Set the callback run each frame.
     * @param fn Callback receiving the owner and frame delta
     */
    void set_update_fn(Update_fn fn);

    /**
     * @brief Set the callback run each fixed step.
     * @param fn Callback receiving the owner and fixed delta
     */
    void set_fixed_update_fn(Fixed_update_fn fn);

    /**
     * @brief Set the callback run on detach.
     * @param fn Callback receiving the owner
     */
    void set_detach_fn(Detach_fn fn);

    /**
     * @brief Invoke the start callback, if set.
     */
    void on_start() override;

    /**
     * @brief Invoke the detach callback, if set.
     */
    void on_detach() override;

    /**
     * @brief Invoke the update callback, if set.
     * @param dt Frame delta time in seconds
     */
    void update(float const dt) override;

    /**
     * @brief Invoke the fixed-update callback, if set.
     * @param fixed_dt Fixed delta time in seconds
     */
    void fixed_update(float const fixed_dt) override;

private:
    Start_fn        _start; ///< Start callback
    Update_fn       _update; ///< Update callback
    Fixed_update_fn _fixed_update; ///< Fixed-update callback
    Detach_fn       _detach; ///< Detach callback
};

} // namespace titan::game
