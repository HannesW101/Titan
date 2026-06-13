#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>
#include <ostream>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Enum UI_anchor
// ----------------------------------------------------------------------------

/**
 * @brief Nine-point anchor for positioning widgets relative to their parent.
 *
 * The anchor defines which point of the parent the widget's offset is measured from.
 */
enum class UI_anchor : std::uint8_t {

    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    MIDDLE_LEFT,
    MIDDLE_CENTER,
    MIDDLE_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
};

// ============================================================================
// Stream operator
// ----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& os, UI_anchor const anchor) {
    switch (anchor) {
        case UI_anchor::TOP_LEFT:      return os << "TOP_LEFT";
        case UI_anchor::TOP_CENTER:    return os << "TOP_CENTER";
        case UI_anchor::TOP_RIGHT:     return os << "TOP_RIGHT";
        case UI_anchor::MIDDLE_LEFT:   return os << "MIDDLE_LEFT";
        case UI_anchor::MIDDLE_CENTER: return os << "MIDDLE_CENTER";
        case UI_anchor::MIDDLE_RIGHT:  return os << "MIDDLE_RIGHT";
        case UI_anchor::BOTTOM_LEFT:   return os << "BOTTOM_LEFT";
        case UI_anchor::BOTTOM_CENTER: return os << "BOTTOM_CENTER";
        case UI_anchor::BOTTOM_RIGHT:  return os << "BOTTOM_RIGHT";
        default:                       return os << "UNKNOWN_ANCHOR";
    }
}

} // namespace titan::ui
