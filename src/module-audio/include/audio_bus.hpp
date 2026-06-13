#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include <ostream>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::audio {

// ============================================================================
// Enum Audio_bus
// ----------------------------------------------------------------------------

/**
 * @brief Logical audio routing channels.
 *
 * Volume and mute are set per-bus. Effective volume = MASTER * bus volume.
 */
enum class Audio_bus : std::uint8_t {

    MASTER, ///< Scales all other buses
    MUSIC, ///< Background music tracks
    SFX, ///< Gameplay sound effects
    UI, ///< Interface sounds (button clicks, menu sounds)
    VOICE, ///< Dialogue and voiceover
    AMBIENT, ///< Environmental ambience

    COUNT, ///< Number of buses - not a valid bus
};
inline std::ostream& operator<<(
    std::ostream&   os,
    Audio_bus const bus
    ) {

    switch (bus) {

        case Audio_bus::MASTER:  return os << "MASTER";
        case Audio_bus::MUSIC:   return os << "MUSIC";
        case Audio_bus::SFX:     return os << "SFX";
        case Audio_bus::UI:      return os << "UI";
        case Audio_bus::VOICE:   return os << "VOICE";
        case Audio_bus::AMBIENT: return os << "AMBIENT";

        default:                 return os << "UNKNOWN BUS";
    }
}

} // namespace titan::audio
