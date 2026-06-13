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
// Class Health
// ----------------------------------------------------------------------------

/**
 * @brief Hit points with damage / heal helpers and death handling.
 *
 * Tracks current and max HP, fires callbacks on damage, heal, and death, and
 * (by default) destroys the owning object when HP reaches zero. The death
 * callback runs before destruction so you can spawn loot, play an animation, or
 * cancel the auto-destroy by calling set_destroy_on_death(false).
 *
 *   auto* hp = obj->add_component<Health>(100);
 *   hp->set_on_death([](Game_object& self){ spawn_explosion(self); });
 *   hp->damage(30);
 */
class Health final : public Game_component {

public:
    using Health_fn = std::function<void(Game_object&)>; ///< Death callback
    using Amount_fn = std::function<void(Game_object&, int amount)>; ///< Damage/heal callback

    /**
     * @brief Construct with default 100 max HP.
     */
    Health() = default;

    /**
     * @brief Construct with a specified max HP (full).
     * @param max_hp Maximum hit points (clamped to >= 1)
     */
    explicit Health(int const max_hp);

    /**
     * @brief Get current hit points.
     * @return Current HP
     */
    int current() const;

    /**
     * @brief Get maximum hit points.
     * @return Max HP
     */
    int max() const;

    /**
     * @brief Check whether the entity is alive.
     * @return True if not dead
     */
    bool is_alive() const;

    /**
     * @brief Check whether the entity is dead.
     * @return True if dead
     */
    bool is_dead() const;

    /**
     * @brief Set the maximum HP.
     * @param max_hp New maximum (clamped to >= 1)
     * @param refill If true, refill to full and clear dead state
     */
    void set_max(int const max_hp, bool const refill = true);

    /**
     * @brief Apply damage; may trigger death and auto-destroy.
     * @param amount Damage amount (ignored if <= 0)
     */
    void damage(int const amount);

    /**
     * @brief Heal up to max (no effect if dead).
     * @param amount Heal amount (ignored if <= 0)
     */
    void heal(int const amount);

    /**
     * @brief Revive and set HP, clearing the dead state.
     * @param to_hp Target HP, or -1 for full
     */
    void revive(int const to_hp = -1);

    /**
     * @brief Set whether reaching 0 HP destroys the owner.
     * @param value True to auto-destroy on death
     */
    void set_destroy_on_death(bool const value);

    /**
     * @brief Check whether death auto-destroys the owner.
     * @return True if auto-destroy is enabled
     */
    bool destroy_on_death() const;

    /**
     * @brief Set the callback fired on damage.
     * @param fn Callback receiving the owner and damage amount
     */
    void set_on_damage(Amount_fn fn);

    /**
     * @brief Set the callback fired on heal.
     * @param fn Callback receiving the owner and heal amount
     */
    void set_on_heal(Amount_fn fn);

    /**
     * @brief Set the callback fired on death.
     * @param fn Callback receiving the owner
     */
    void set_on_death(Health_fn fn);

private:
    int  _max              = 100; ///< Maximum HP
    int  _current          = 100; ///< Current HP
    bool _dead             = false; ///< Dead state
    bool _destroy_on_death = true; ///< Auto-destroy on death

    Amount_fn _on_damage;   ///< Damage callback
    Amount_fn _on_heal;     ///< Heal callback
    Health_fn _on_death;    ///< Death callback

    /**
     * @brief Check for death and fire death handling if HP hit zero.
     */
    void _check_death();
};

} // namespace titan::game
