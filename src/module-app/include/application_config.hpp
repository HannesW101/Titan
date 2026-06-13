#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"

#include <cstdint>
#include <string>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::core { class Config; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::app {

// ============================================================================
// Struct Application_config
// ----------------------------------------------------------------------------

/**
 * @brief Settings that define how the Application creates its window and runs
 * its main loop. Construct one, tweak fields, and pass to Application.
 *
 * Can be loaded from / saved to a titan::core::Config so the window size,
 * fullscreen flag, vsync, and frame caps live in the user's settings file.
 */
struct Application_config final {

    // ---- Window ----
    std::string  title        = "Titan";
    unsigned int width        = 1280u;
    unsigned int height       = 720u;
    bool         fullscreen   = false;
    bool         resizable    = true;
    unsigned int antialiasing = 0u; ///< MSAA level (0 = off)

    // ---- Frame timing ----
    bool         vsync           = true;
    unsigned int framerate_limit = 0u; ///< 0 = uncapped (ignored if vsync)

    /**
     * @brief Fixed timestep for deterministic update(), in seconds.
     * The loop runs zero or more fixed_update() steps per frame so physics and
     * gameplay stay frame-rate independent. 1/60 is the usual default.
     */
    float fixed_timestep = 1.0f / 60.0f;

    /**
     * @brief Maximum fixed steps per frame, preventing the "spiral of death"
     * if a frame hitches (e.g. debugger pause). Excess time is dropped.
     */
    unsigned int max_fixed_steps_per_frame = 5u;

    // ---- Rendering ----
    sf::Color clear_color = sf::Color(18u, 18u, 28u);

    // ---- Config (de)serialization ----

    /**
     * @brief Populate this config from a Config section.
     *
     * Looks up keys: title, width, height, fullscreen, resizable, vsync,
     * framerate_limit, fixed_timestep, antialiasing under [application].
     * @param config Config to read from
     */
    void load_from_config(core::Config const& config);

    /**
     * @brief Write current settings into a Config under [application].
     * @param config Config to write into
     */
    void save_to_config(core::Config& config) const;
};

} // namespace titan::app
