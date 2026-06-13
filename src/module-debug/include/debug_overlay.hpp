#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/System/Vector2.hpp"

#include <deque>
#include <string>
#include <vector>

// ============================================================================
// Forward declarations (1/2)
// ----------------------------------------------------------------------------

namespace titan::render { class Renderer; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::debug {

// ============================================================================
// Forward declarations (2/2)
// ----------------------------------------------------------------------------

class Profiler;

// ============================================================================
// Class Debug_overlay
// ----------------------------------------------------------------------------

/**
 * @brief On-screen developer overlay: FPS / frame-time readout, a frame-time
 * graph, profiler scope timings, and arbitrary user "watch" lines.
 *
 * Pulls numbers from a Profiler and draws them on the renderer's UI_OVERLAY
 * layer with a font loaded from the Resource_manager. Toggle with set_visible()
 * (e.g. bound to F3). Keepalive drawables are held internally to satisfy the
 * renderer's outlive-end_frame() contract.
 *
 *   Debug_overlay overlay;
 *   overlay.set_font("default_font");
 *   // each frame, after profiler.end_frame():
 *   overlay.set_watch("objects", std::to_string(world.count()));
 *   overlay.render(renderer, profiler);
 */
class Debug_overlay final {

public:
    /**
     * @brief Construct a visible overlay with default styling.
     */
    Debug_overlay() = default;

    /**
     * @brief Set the font resource used for text.
     * @param font_resource Font key in the Resource_manager
     */
    void set_font(std::string const font_resource);

    /**
     * @brief Show or hide the overlay.
     * @param visible True to show
     */
    void set_visible(bool const visible);

    /**
     * @brief Check whether the overlay is visible.
     * @return True if visible
     */
    bool is_visible() const;

    /**
     * @brief Toggle overlay visibility.
     */
    void toggle();

    /**
     * @brief Set the overlay's top-left position.
     * @param pos Position in pixels
     */
    void set_position(sf::Vector2f const pos);

    /**
     * @brief Set the text color.
     * @param color Text color
     */
    void set_text_color(sf::Color const color);

    /**
     * @brief Set the background panel color (alpha supported).
     * @param color Background color
     */
    void set_background(sf::Color const color);

    /**
     * @brief Show profiler scope timings (physics/render/etc.) in the overlay.
     */
    /**
     * @brief Show or hide profiler scope timings in the overlay.
     * @param show True to list scope timings
     */
    void set_show_scopes(bool const show);

    /**
     * @brief Set / update a custom watch line shown in the overlay.
     */
    /**
     * @brief Set or update a custom watch line.
     * @param key Watch label
     * @param value Watch value
     */
    void set_watch(std::string const& key, std::string const& value);

    /**
     * @brief Remove all custom watch lines.
     */
    void clear_watches();

    /**
     * @brief Draw the overlay. No-op when hidden.
     */
    /**
     * @brief Draw the overlay. No-op when hidden.
     * @param renderer Renderer to submit to
     * @param profiler Profiler to read stats from
     */
    void render(render::Renderer& renderer, Profiler const& profiler);

private:
    std::string  _font_name   = "default_font";
    bool         _visible     = true;
    bool         _show_scopes = true;
    sf::Vector2f _position    = { 8.0f, 8.0f };
    sf::Color    _text_color  = sf::Color::White;
    sf::Color    _bg_color    = sf::Color(0, 0, 0, 150);

    std::deque<float>                                _frame_history;
    std::vector<std::pair<std::string, std::string>> _watches;

    // Keepalive stores so submitted drawables outlive end_frame() (renderer
    // contract). Instance members, not function statics, so multiple overlays
    // don't clobber each other.
    std::deque<sf::Text>            _text_keepalive;
    std::deque<sf::RectangleShape>  _rect_keepalive;
    sf::VertexArray                 _graph_keepalive;

    static constexpr std::size_t MAX_HISTORY = 120u;
};

} // namespace titan::debug
