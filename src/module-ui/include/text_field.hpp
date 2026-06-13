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
// Class Text_field
// ----------------------------------------------------------------------------

/** @brief Single-line editable text input. Click to focus, type to edit,
 *  Enter to submit, Backspace to delete. Supports placeholder + password. */
class Text_field final : public Widget {

public:
    /**
     * @brief Construct a Text_field.
     * @param name Widget name (unique within its parent)
     */
    explicit Text_field(std::string const& name);
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
     * @brief Set the max length.
     * @param n New max length
     */
    void set_max_length(std::size_t n);
    /**
     * @brief Set the password.
     * @param on New password
     */
    void set_password(bool on);
    /**
     * @brief Set the "on submit" callback.
     * @param cb Callback to invoke
     */
    void set_on_submit(std::function<void(std::string const&)> cb);
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
    std::string _text,_placeholder;
    std::size_t _max=256u;
    float _blink=0.f;
    bool _password=false;
    std::function<void(std::string const&)> _on_submit,_on_changed;
    std::uint64_t _text_cb=0,_enter_cb=0,_bs_cb=0;
};
} // namespace titan::ui
