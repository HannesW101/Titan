#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <chrono>
#include <cstdint>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace titan::debug {

// ============================================================================
// Class Profiler
// ----------------------------------------------------------------------------

/**
 * @brief Lightweight per-frame timing: a rolling FPS/frame-time average plus
 * named scope timers for measuring sections of a frame.
 *
 * Usage each frame:
 *   profiler.begin_frame();
 *   { TITAN_PROFILE_SCOPE(profiler, "physics"); world.fixed_update(dt); }
 *   { TITAN_PROFILE_SCOPE(profiler, "render");  world.render(r); }
 *   profiler.end_frame();
 *
 * Then read fps(), average_frame_ms(), or scope_ms("physics") for display in a
 * Debug_overlay or logs. A singleton accessor is provided for convenience so
 * scope macros work without threading a reference everywhere.
 */
class Profiler final {

public:
    using Clock = std::chrono::steady_clock;

    /**
     * @brief Construct a profiler with the clock started.
     */
    Profiler();

    /**
     * @brief Get the shared profiler instance.
     * @return Reference to the singleton profiler
     */
    static Profiler& instance();

    // ---- Frame timing ------------------------------------------------------

    /**
     * @brief Mark the start of a frame.
     */
    void begin_frame();

    /**
     * @brief Mark the end of a frame and update timing stats.
     */
    void end_frame();

    /**
     * @brief Get the smoothed frames per second.
     * @return Frames per second
     */
    float fps() const;

    /**
     * @brief Get the smoothed frame time.
     * @return Average frame time in milliseconds
     */
    float average_frame_ms() const;

    /**
     * @brief Get the most recent frame time.
     * @return Last frame time in milliseconds
     */
    float last_frame_ms() const;

    /**
     * @brief Get the number of completed frames.
     * @return Frame index
     */
    std::uint64_t frame_index() const;

    // ---- Named scopes ------------------------------------------------------

    /**
     * @brief Begin timing a named scope.
     * @param name Scope name
     */
    void begin_scope(std::string const& name);

    /**
     * @brief End timing a named scope.
     * @param name Scope name
     */
    void end_scope(std::string const& name);

    /**
     * @brief Get the last measured time for a scope.
     * @param name Scope name
     * @return Last duration in milliseconds (0 if unknown)
     */
    float scope_ms(std::string const& name) const;

    /**
     * @brief All current scope timings (name -> last ms), for overlay listing.
     */
    /**
     * @brief Get all current scope timings.
     * @return Vector of (name, last milliseconds) pairs
     */
    std::vector<std::pair<std::string, float>> scopes() const;

    // ---- Config ------------------------------------------------------------

    /**
     * @brief Set the smoothing factor for averaged timings.
     * @param factor EMA factor in (0,1]; 1 = no smoothing
     */
    void set_smoothing(float const factor);

    /**
     * @brief Enable or disable the profiler.
     * @param enabled True to enable timing
     */
    void set_enabled(bool const enabled);

    /**
     * @brief Check whether the profiler is enabled.
     * @return True if enabled
     */
    bool is_enabled() const;

    /**
     * @brief Reset all timings and counters.
     */
    void reset();

private:
    struct Scope_data {

        Clock::time_point start;
        float             last_ms = 0.0f;
        bool              open    = false;
    };

    bool  _enabled   = true;
    float _smoothing = 0.1f;

    Clock::time_point _frame_start;
    float             _last_frame_ms = 0.0f;
    float             _avg_frame_ms  = 0.0f;
    std::uint64_t     _frame_index   = 0u;

    std::unordered_map<std::string, Scope_data> _scopes;
};

// ============================================================================
// Scope helper
// ----------------------------------------------------------------------------

/**
 * @brief RAII scope timer: times from construction to destruction.
 */
class Profile_scope final {
public:
    /**
     * @brief Begin timing a scope on construction.
     * @param profiler Profiler to record into
     * @param name Scope name
     */
    Profile_scope(Profiler& profiler, std::string name);

    /**
     * @brief End timing the scope on destruction.
     */
    ~Profile_scope();

    Profile_scope           (Profile_scope const&) = delete;
    Profile_scope& operator=(Profile_scope const&) = delete;
    Profile_scope           (Profile_scope&&     ) = delete;
    Profile_scope& operator=(Profile_scope&&     ) = delete;

private:
    Profiler&   _profiler;
    std::string _name;
};

} // namespace titan::debug

// ============================================================================
// Macros
// ----------------------------------------------------------------------------

// Convenience macro: times the enclosing block under the given name.
#define TITAN_PROFILE_CONCAT_INNER(a, b) a##b

#define TITAN_PROFILE_CONCAT(a, b) TITAN_PROFILE_CONCAT_INNER(a, b)

#define TITAN_PROFILE_SCOPE(profiler, name) ::titan::debug::Profile_scope TITAN_PROFILE_CONCAT(_titan_profile_scope_, __LINE__)((profiler), (name))
