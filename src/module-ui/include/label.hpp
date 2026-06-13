#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Label
// ----------------------------------------------------------------------------

/** @brief A non-interactive text display with alignment and optional wrap. */
class Label final : public Widget {

public:
    /**
     * @brief Construct a Label.
     * @param name Widget name (unique within its parent)
     */
    explicit Label(std::string const& name);
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
     * @brief Set the align.
     * @param a New align
     */
    void set_align(Text_align a);
    /**
     * @brief Set the wrap.
     * @param wrap New wrap
     */
    void set_wrap(bool wrap);

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
    std::string _text;
    Text_align  _align = Text_align::LEFT;
    bool        _wrap  = false;
};

} // namespace titan::ui
