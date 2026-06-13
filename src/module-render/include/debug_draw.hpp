#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-render/include/render_layer.hpp"

#include "SFML/Graphics.hpp"

#include <string>
#include <vector>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::render { class Renderer; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::render {

// ============================================================================
// Class Debug_draw
// ----------------------------------------------------------------------------

/**
 * @brief Immediate-mode debug drawing.
 *
 * Draws lines, boxes, circles, text to the DEBUG render layer.
 * All submissions are cleared each frame.
 * Only active when enabled.
 *
 * @code
 * debug.line({0,0}, {100,100}, sf::Color::Red);
 * debug.box(collider_rect, sf::Color::Green);
 * debug.text({50,50}, "hp=42");
 * @endcode
 */
class Debug_draw final {

public:
    /**
     * @brief Construct with renderer and font for text rendering.
     * @param renderer Renderer to submit to
     * @param font Font for debug text
     */
    Debug_draw(Renderer& renderer, sf::Font const& font);

    /**
     * @brief Enable or disable debug drawing.
     * @param flag Enabled state
     */
    void set_enabled(bool const flag);

    /**
     * @brief Check if debug drawing is enabled.
     * @return Enabled state
     */
    bool is_enabled() const;

    // -------------------------------------------------------------------------
    // Shapes
    // -------------------------------------------------------------------------

    /**
     * @brief Draw a line between two points.
     */
    void line(sf::Vector2f const a, sf::Vector2f const b, sf::Color const color);

    /**
     * @brief Draw a hollow rectangle.
     */
    void box(sf::FloatRect const rect, sf::Color const color);

    /**
     * @brief Draw a filled rectangle.
     */
    void filled_box(sf::FloatRect const rect, sf::Color const color);

    /**
     * @brief Draw a hollow circle.
     * @param segments Number of line segments
     */
    void circle(sf::Vector2f const center, float const radius, sf::Color const color, int const segments = 24);

    /**
     * @brief Draw a cross/plus at a point.
     */
    void cross(sf::Vector2f const center, float const radius, sf::Color const color);

    /**
     * @brief Draw an arrow from one point to another.
     */
    void arrow(sf::Vector2f const from, sf::Vector2f const to, sf::Color const color);

    // -------------------------------------------------------------------------
    // Text
    // -------------------------------------------------------------------------

    /**
     * @brief Draw debug text at a world position.
     */
    void text(
        sf::Vector2f const position,
        std::string const& message,
        sf::Color const color = sf::Color::White,
        unsigned int const char_size = 12u
        );

    // -------------------------------------------------------------------------
    // Frame
    // -------------------------------------------------------------------------

    /**
     * @brief Submit all accumulated geometry to the renderer. Called by game loop.
     */
    void flush();

    /**
     * @brief Clear all accumulated geometry.
     */
    void clear();

private:
    Renderer&       _renderer;
    sf::Font const& _font;
    bool            _enabled;

    sf::VertexArray       _lines;   ///< Accumulated line geometry
    sf::VertexArray       _filled;  ///< Accumulated filled geometry
    std::vector<sf::Text> _texts;   ///< Accumulated text drawables
};

} // namespace titan::render
