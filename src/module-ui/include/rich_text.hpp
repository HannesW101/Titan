#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"
#include "SFML/Graphics/Color.hpp"

#include <string>
#include <vector>

namespace titan::ui {

// ============================================================================
// Class Rich_text
// ----------------------------------------------------------------------------

/** @brief Text composed of styled spans (per-span color + size), laid out
 *  left-to-right with wrapping. Build with add_span(); good for log windows,
 *  dialogue with colored names, damage numbers, etc. */
class Rich_text final : public Widget {

public:
// ============================================================================
// Class Span
// ----------------------------------------------------------------------------

    struct Span { std::string text; sf::Color color=sf::Color::White; float size=14.f; };
    /**
     * @brief Construct a Rich_text.
     * @param name Widget name (unique within its parent)
     */
    explicit Rich_text(std::string const& name);
    /**
     * @brief Clear spans.
     */
    void clear_spans();
    /**
     * @brief Add span.
     * @param text text
     * @param color color
     * @param size size
     */
    void add_span(std::string const& text, sf::Color color, float size = 14.f);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "label"; }

private:
    std::vector<Span> _spans;
};
} // namespace titan::ui
