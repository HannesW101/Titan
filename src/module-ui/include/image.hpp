#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"

#include <optional>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Image
// ----------------------------------------------------------------------------

/** @brief Displays a texture (optionally a sub-rect), with tint + aspect. */
class Image final : public Widget {

public:
    /**
     * @brief Construct a Image.
     * @param name Widget name (unique within its parent)
     */
    explicit Image(std::string const& name);
    /**
     * @brief Set the texture.
     * @param resource_name New texture
     */
    void set_texture(std::string const& resource_name);
    /**
     * @brief Set the uv rect.
     * @param rect New uv rect
     */
    void set_uv_rect(sf::IntRect rect);
    /**
     * @brief Clear uv rect.
     */
    void clear_uv_rect();
    /**
     * @brief Set the tint.
     * @param c New tint
     */
    void set_tint(sf::Color c);
    /**
     * @brief Set the preserve aspect.
     * @param p New preserve aspect
     */
    void set_preserve_aspect(bool p);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    std::string                _tex;
    std::optional<sf::IntRect> _uv;
    sf::Color                  _tint   = sf::Color::White;
    bool                       _aspect = true;
};
} // namespace titan::ui
