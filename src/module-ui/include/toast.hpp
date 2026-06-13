#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

namespace titan::ui {

// ============================================================================
// Class Toast
// ----------------------------------------------------------------------------

/** @brief Brief notification that auto-fades out. */
class Toast final : public Widget {

public:
    /**
     * @brief Construct a Toast.
     * @param name Widget name (unique within its parent)
     */
    explicit Toast(std::string const& name);
    /**
     * @brief Set the text.
     * @param t New text
     */
    void set_text(std::string const& t);
    /**
     * @brief Show.
     * @param duration_seconds duration seconds
     */
    void show(float duration_seconds = 3.f);

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
    std::string style_key() const override { return "toast"; }
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    std::string _text; float _time=0.f,_duration=0.f,_op=0.f;
};
} // namespace titan::ui
