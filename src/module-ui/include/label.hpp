#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>
#include <vector>

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

    float content_height() const override;

protected:
    void on_layout() override;
    void on_render(render::Renderer& renderer) override;
    std::string style_key() const override { return "label"; }

private:
    void _build_lines(sf::Font const& font, unsigned int csize, float wrap_width);

    std::string              _text;
    Text_align               _align = Text_align::LEFT;
    bool                     _wrap  = false;
    std::vector<std::string> _lines;
    float                    _cached_width = -1.0f;
    std::string              _cached_text;
};

} // namespace titan::ui
