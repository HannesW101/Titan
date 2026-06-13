#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

namespace titan::ui {

// ============================================================================
// Class Tooltip
// ----------------------------------------------------------------------------

/** @brief Hover popup that fades in after a delay and follows the mouse. */
class Tooltip final : public Widget {

public:
    /**
     * @brief Construct a Tooltip.
     * @param name Widget name (unique within its parent)
     */
    explicit Tooltip(std::string const& name);
    /**
     * @brief Set the text.
     * @param t New text
     */
    void set_text(std::string const& t);
    /**
     * @brief Set the delay.
     * @param seconds New delay
     */
    void set_delay(float seconds);
    /**
     * @brief Attach to.
     * @param target target
     */
    void attach_to(Widget* target);

protected:
    /**
     * @brief Handle the update event.
     * @param dt dt
     */
    void on_update(float dt) override;
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "tooltip"; }
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    std::string _text; Widget* _target=nullptr; float _delay=0.5f,_hover=0.f,_op=0.f;
};
} // namespace titan::ui
