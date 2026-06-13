#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-core/time/include/timer.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::core {

// ============================================================================
// Class Time_manager
// ----------------------------------------------------------------------------

/**
 * @brief Singleton manager for multiple named timers.
 *
 * Allows independent timing for gameplay, UI, physics, particles, etc.
 * Each timer can have different time scales and pause states.
 */
class Time_manager final {

public:
    Time_manager           (Time_manager const&) = delete;
    Time_manager& operator=(Time_manager const&) = delete;
    Time_manager           (Time_manager&&     ) = delete;
    Time_manager& operator=(Time_manager&&     ) = delete;

    /**
     * @brief Get singleton instance.
     * @return Reference to Time_manager instance
     */
    static Time_manager& instance();

    /**
     * @brief Create or get a named timer.
     * @param name Timer identifier
     * @return Reference to timer
     */
    Timer& get_timer(std::string_view const name);

    /**
     * @brief Get timer if it exists.
     * @param name Timer identifier
     * @return Optional reference to timer
     */
    std::optional<std::reference_wrapper<Timer>> try_get_timer(std::string_view const name);

    /**
     * @brief Check if named timer exists.
     * @param name Timer identifier
     * @return True if timer exists
     */
    bool has_timer(std::string_view const name) const;

    /**
     * @brief Remove a named timer.
     * @param name Timer identifier
     */
    void remove_timer(std::string_view const name);

    /**
     * @brief Update all timers.
     */
    void tick_all();

    /**
     * @brief Pause all timers.
     */
    void pause_all();

    /**
     * @brief Resume all timers.
     */
    void resume_all();

    /**
     * @brief Get the main default timer.
     * @return Reference to main timer
     */
    Timer& main();

private:
    Time_manager();

    ~Time_manager() = default;

    std::unordered_map<std::string, Timer> _timers; ///< Named timer storage
};

} // namespace titan::core
