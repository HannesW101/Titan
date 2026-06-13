#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include <cstdint>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Struct UI_length
// ----------------------------------------------------------------------------

/**
 * @brief A length value that can be either absolute pixels or a percentage
 * of the parent's corresponding dimension.
 */
struct UI_length {

    enum class Type : std::uint8_t {
        PIXELS,
        PERCENT,
    };

    float value = 0.0f;
    Type  type  = Type::PIXELS;

    [[nodiscard]] static constexpr UI_length px(float const pixels) {
        return { pixels, Type::PIXELS };
    }

    [[nodiscard]] static constexpr UI_length pct(float const fraction) {
        return { fraction, Type::PERCENT };
    }

    [[nodiscard]] constexpr float resolve(float const parent_dim) const {
        return (type == Type::PIXELS) ? value : value * parent_dim;
    }
};

} // namespace titan::ui
