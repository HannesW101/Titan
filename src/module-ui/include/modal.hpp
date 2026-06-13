#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstdint>
#include <functional>
#include <string>

namespace titan::ui {

// ============================================================================
// Class Modal
// ----------------------------------------------------------------------------

/** @brief Dimmed-backdrop dialog. show()/hide(); click backdrop closes if set. */
class Modal final : public Widget {

public:
    /**
     * @brief Construct a Modal.
     * @param name Widget name (unique within its parent)
     */
    explicit Modal(std::string const& name);
    /**
     * @brief Set the title.
     * @param t New title
     */
    void set_title(std::string const& t);
    /**
     * @brief Set the closeable.
     * @param c New closeable
     */
    void set_closeable(bool c);
    /**
     * @brief Show.
     */
    void show(); void hide(); bool is_shown() const;
    /**
     * @brief Set the "on close" callback.
     * @param cb Callback to invoke
     */
    void set_on_close(std::function<void()> cb);

protected:
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
    std::string style_key() const override { return "panel"; }
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    std::string _title; bool _shown=false,_closeable=true; float _title_h=32.f;
    std::function<void()> _on_close;
};
} // namespace titan::ui
