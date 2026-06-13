#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"
#include "SFML/System/Vector2.hpp"

#include <functional>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Icon_button
// ----------------------------------------------------------------------------

/** @brief Button showing a texture icon centered in its rect. */
class Icon_button final : public Widget {

public:
    /**
     * @brief Construct a Icon_button.
     * @param name Widget name (unique within its parent)
     */
    explicit Icon_button(std::string const& name);
    /**
     * @brief Set the icon.
     * @param texture_name New icon
     */
    void set_icon(std::string const& texture_name);
    void set_icon_size(sf::Vector2f size); // {0,0} = native
    /**
     * @brief Set the icon tint.
     * @param c New icon tint
     */
    void set_icon_tint(sf::Color c);
    /**
     * @brief Set the "on click" callback.
     * @param cb Callback to invoke
     */
    void set_on_click(std::function<void()> cb);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Handle the clicked event.
     */
    void on_clicked() override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "button"; }

private:
    std::string           _icon;
    sf::Vector2f          _icon_size{ 0.f, 0.f };
    sf::Color             _tint = sf::Color::White;
    std::function<void()> _on_click;
};

} // namespace titan::ui
