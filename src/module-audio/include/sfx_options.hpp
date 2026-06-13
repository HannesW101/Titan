#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics.hpp"

#include <optional>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::audio {

// ============================================================================
// Struct Sfx_options
// ----------------------------------------------------------------------------

/**
 * @brief Options for playing a sound effect.
 */
struct Sfx_options {

    float volume        = 1.0f; ///< Volume scalar [0, 1] before bus scaling
    float pitch         = 1.0f; ///< Pitch multiplier (1.0 = normal)
    float pitch_jitter  = 0.0f; ///< Random +/- range added to pitch each play
    bool  loop          = false; ///< Whether to loop the sound

    /// World position for spatial attenuation. Empty = non-spatial (no panning)
    std::optional<sf::Vector2f> world_position;

    float falloff_radius = 500.0f; ///< Distance at which volume reaches zero
};

} // namespace titan::audio
