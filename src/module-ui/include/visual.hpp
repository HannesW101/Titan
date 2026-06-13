#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"

#include <cstdint>
#include <optional>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Struct Visual
// ----------------------------------------------------------------------------

/**
 * @brief A single drawable appearance for a widget background.
 *
 * A Visual can be: nothing, a solid/outlined color rect, a stretched texture,
 * or a nine-slice texture. This is the single flexible primitive every widget
 * uses to draw itself - no widget hardcodes its background anymore.
 */
struct Visual {

    enum class Kind : std::uint8_t {
        NONE,        ///< Draw nothing
        COLOR,       ///< Solid fill + optional outline
        TEXTURE,     ///< Stretched/fitted texture
        NINE_SLICE,  ///< Nine-slice stretchable texture
    };

    Kind kind = Kind::NONE;

    // COLOR
    sf::Color fill_color    = sf::Color(50, 50, 50, 200);
    sf::Color outline_color = sf::Color(100, 100, 100, 255);
    float     outline_thickness = 0.0f;

    // TEXTURE / NINE_SLICE
    std::string texture_name;                 ///< Resource_manager key
    sf::Color   tint = sf::Color::White;       ///< Multiplied with texture
    std::optional<sf::IntRect> uv_rect;        ///< Sub-region (atlas), texture only
    bool        preserve_aspect = false;       ///< Texture only

    // NINE_SLICE borders (in texture pixels)
    float border_top    = 8.0f;
    float border_right  = 8.0f;
    float border_bottom = 8.0f;
    float border_left   = 8.0f;

    // ---- Builders for ergonomic construction -------------------------------

    static Visual none() {
        Visual v;
        v.kind = Kind::NONE;
        return v;
    }

    static Visual color(
        sf::Color fill,
        sf::Color outline = sf::Color::Transparent,
        float thickness = 0.0f
        ) {
        Visual v;
        v.kind              = Kind::COLOR;
        v.fill_color        = fill;
        v.outline_color     = outline;
        v.outline_thickness = thickness;
        return v;
    }

    static Visual texture(
        std::string name,
        sf::Color tint_color = sf::Color::White,
        bool preserve = false
        ) {
        Visual v;
        v.kind            = Kind::TEXTURE;
        v.texture_name    = std::move(name);
        v.tint            = tint_color;
        v.preserve_aspect = preserve;
        return v;
    }

    static Visual nine_slice(
        std::string name,
        float top, float right, float bottom, float left,
        sf::Color tint_color = sf::Color::White
        ) {
        Visual v;
        v.kind          = Kind::NINE_SLICE;
        v.texture_name  = std::move(name);
        v.tint          = tint_color;
        v.border_top    = top;
        v.border_right  = right;
        v.border_bottom = bottom;
        v.border_left   = left;
        return v;
    }
};

} // namespace titan::ui
