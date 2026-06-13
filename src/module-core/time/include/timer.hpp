#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics.hpp"

#include <cstdint>
#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Class Timer
// ----------------------------------------------------------------------------

/**
 * @brief Individual timer tracking elapsed time, delta, and fixed-step accumulation.
 *
 * Each timer maintains its own clock, time scale, frame count, and fixed-step
 * accumulator. Supports pause/resume and time scaling for slow-motion or
 * fast-forward effects.
 */
class Timer final {

public:
    /**
     * @brief Construct timer with default settings.
     */
    Timer();

    /**
     * @brief Update timer for current frame.
     *
     * Measures elapsed time since last tick, applies scaling and clamping,
     * accumulates fixed-step time for physics updates.
     */
    void tick();

    /**
     * @brief Reset timer to initial state.
     */
    void reset();

    /**
     * @brief Pause the timer (delta becomes 0 until resumed).
     */
    void pause();

    /**
     * @brief Resume a paused timer.
     */
    void resume();

    /**
     * @brief Check if timer is currently paused.
     * @return True if paused
     */
    bool is_paused() const;

    /**
     * @brief Get frame delta time with scaling and clamping applied.
     * @return Delta time in seconds
     */
    float delta_time() const;

    /**
     * @brief Get raw frame delta without scaling or clamping.
     * @return Unscaled delta time in seconds
     */
    float unscaled_delta_time() const;

    /**
     * @brief Get total elapsed time with scaling applied.
     * @return Total time in seconds
     */
    double total_time() const;

    /**
     * @brief Get total elapsed time without scaling.
     * @return Unscaled total time in seconds
     */
    double unscaled_total_time() const;

    /**
     * @brief Get number of frames processed by this timer.
     * @return Frame count
     */
    std::uint64_t frame_count() const;

    /**
     * @brief Set time scale multiplier.
     * @param scale Time scale (0=paused, 0.5=half speed, 1=normal, 2=double speed)
     */
    void set_time_scale(float const scale);

    /**
     * @brief Get current time scale.
     * @return Time scale multiplier
     */
    float time_scale() const;

    /**
     * @brief Set maximum delta clamp to prevent huge time spikes.
     * @param seconds Maximum delta in seconds
     */
    void set_max_delta(float const seconds);

    /**
     * @brief Get maximum delta clamp value.
     * @return Max delta in seconds
     */
    float max_delta() const;

    /**
     * @brief Set fixed timestep for physics updates.
     * @param seconds Fixed step in seconds (e.g., 1/60 for 60Hz physics)
     */
    void set_fixed_dt(float const seconds);

    /**
     * @brief Get fixed timestep value.
     * @return Fixed delta in seconds
     */
    float fixed_dt() const;

    /**
     * @brief Consume one fixed timestep from accumulator if available.
     * @return True if a fixed step was consumed, false if accumulator empty
     */
    bool consume_fixed_step();

private:
    sf::Clock _real_clock; ///< SFML clock for measuring real time

    float         _delta; ///< Scaled and clamped delta time
    float         _unscaled_delta; ///< Raw delta time
    double        _total; ///< Total scaled elapsed time
    double        _unscaled_total; ///< Total unscaled elapsed time
    std::uint64_t _frame_count; ///< Number of ticks processed

    float _time_scale; ///< Time scaling multiplier
    float _max_delta; ///< Maximum delta clamp (prevents spikes)

    float _fixed_dt; ///< Fixed timestep size
    float _fixed_accumulator; ///< Accumulated time for fixed steps

    bool _paused; ///< Whether timer is paused
};

} // namespace titan::core
