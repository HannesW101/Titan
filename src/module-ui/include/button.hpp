#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <functional>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Button
// ----------------------------------------------------------------------------

/** @brief Clickable push button with text and per-state visuals. */
class Button final : public Widget {

public:
    /**
     * @brief Construct a Button.
     * @param name Widget name (unique within its parent)
     */
    explicit Button(std::string const& name);
    /**
     * @brief Set the text.
     * @param text New text
     */
    void set_text(std::string const& text);
    /**
     * @brief Get the text.
     * @return The text
     */
    std::string const& text() const;
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
    std::string           _text;
    std::function<void()> _on_click;
};

} // namespace titan::ui
