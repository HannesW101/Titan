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
// Enum Widget_state
// ----------------------------------------------------------------------------

/**
 * @brief Interaction state of a widget. Drives which Visual is shown and
 * which text color is used.
 */
enum class Widget_state : std::uint8_t {
    NORMAL,
    HOVERED,
    PRESSED,
    FOCUSED,
    DISABLED,

    COUNT,
};

inline std::ostream& operator<<(std::ostream& os, Widget_state const s) {
    switch (s) {
        case Widget_state::NORMAL:   return os << "NORMAL";
        case Widget_state::HOVERED:  return os << "HOVERED";
        case Widget_state::PRESSED:  return os << "PRESSED";
        case Widget_state::FOCUSED:  return os << "FOCUSED";
        case Widget_state::DISABLED: return os << "DISABLED";
        default:                     return os << "UNKNOWN_STATE";
    }
}

} // namespace titan::ui
