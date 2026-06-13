#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include <ostream>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::render {

// ============================================================================
// Enums
// ----------------------------------------------------------------------------

/**
 * @brief Drawing order layers. Lower values draw first (further back).
 *
 * Within a layer, items sort by sort_key (lower = further back).
 * BACKGROUND..FOREGROUND use the world camera.
 * UI..DEBUG use the UI camera.
 */
enum class Render_layer : std::uint8_t {

    BACKGROUND, ///< Skybox, far parallax
    WORLD_TILES, ///< Tilemap
    WORLD_OBJECTS, ///< Static decoration
    ENTITIES, ///< Characters, dynamic objects
    PARTICLES, ///< Particle effects
    FOREGROUND, ///< Near parallax, overlays
    UI, ///< HUD
    UI_OVERLAY, ///< Modals, pause menu
    DEBUG, ///< Bounding boxes, gizmos

    COUNT,
};
inline std::ostream& operator<<(
    std::ostream& os,
    Render_layer const layer
    ) {

    switch (layer) {

        case Render_layer::BACKGROUND:    return os << "BACKGROUND";
        case Render_layer::WORLD_TILES:   return os << "WORLD_TILES";
        case Render_layer::WORLD_OBJECTS: return os << "WORLD_OBJECTS";
        case Render_layer::ENTITIES:      return os << "ENTITIES";
        case Render_layer::PARTICLES:     return os << "PARTICLES";
        case Render_layer::FOREGROUND:    return os << "FOREGROUND";
        case Render_layer::UI:            return os << "UI";
        case Render_layer::UI_OVERLAY:    return os << "UI_OVERLAY";
        case Render_layer::DEBUG:         return os << "DEBUG";

        default:                          return os << "UNKNOWN LAYER";
    }
}

// ============================================================================
// Constants
// ----------------------------------------------------------------------------

constexpr std::uint8_t LAYER_COUNT = static_cast<std::uint8_t>(Render_layer::COUNT);

} // namespace titan::render
