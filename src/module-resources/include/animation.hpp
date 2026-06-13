#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics.hpp"

#include <string>
#include <vector>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::resources {

// ============================================================================
// Enums
// ----------------------------------------------------------------------------

/**
 * @brief Playback mode for an animation.
 */
enum class Animation_mode {

    LOOP, ///< Loop back to start when last frame is reached
    ONE_SHOT, ///< Play once and stop on last frame
    HOLD, ///< Play once and hold on last frame indefinitely
    PING_PONG, ///< Play forward then backward repeatedly
};

// ============================================================================
// Structs
// ----------------------------------------------------------------------------

/**
 * @brief A named animation sequence with configurable playback.
 */
struct Animation {

    std::string              name; ///< Animation name (e.g. "idle", "run")
    std::vector<sf::IntRect> frames; ///< Texture rects for each frame
    float                    fps  = 12.0f; ///< Frames per second
    Animation_mode           mode = Animation_mode::LOOP; ///< Playback mode
};

} // namespace titan::resources
