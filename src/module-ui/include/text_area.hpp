#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

namespace titan::ui {

// ============================================================================
// Class Text_area
// ----------------------------------------------------------------------------

/** @brief Multi-line editable text box with wrapping and wheel scrolling. */
class Text_area final : public Widget {

public:
    /**
     * @brief Construct a Text_area.
     * @param name Widget name (unique within its parent)
     */
    explicit Text_area(std::string const& name);
    /**
     * @brief Set the text.
     * @param t New text
     */
    void set_text(std::string const& t);
    /**
     * @brief Get the text.
     * @return The text
     */
    std::string const& text() const;
    /**
     * @brief Set the placeholder.
     * @param p New placeholder
     */
    void set_placeholder(std::string const& p);
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(std::string const&)> cb);

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
     * @brief Handle the pressed event.
     */
    void on_pressed() override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "text_field"; }

private:
    std::string _text,_placeholder; float _blink=0.f,_scroll=0.f;
    std::function<void(std::string const&)> _on_changed;
    std::uint64_t _text_cb=0,_enter_cb=0,_bs_cb=0,_up=0,_dn=0;
};
} // namespace titan::ui
