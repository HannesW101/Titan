#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

namespace titan::ui {

// ============================================================================
// Class Progress_bar
// ----------------------------------------------------------------------------

/** @brief Animated fill bar [0,1], horizontal or vertical, optional % label. */
class Progress_bar final : public Widget {

public:
    enum class Orientation { HORIZONTAL, VERTICAL };
    /**
     * @brief Construct a Progress_bar.
     * @param name Widget name (unique within its parent)
     */
    explicit Progress_bar(std::string const& name);
    /**
     * @brief Set the value.
     * @param v New value
     */
    void set_value(float v);
    /**
     * @brief Get the value.
     * @return The value
     */
    float value() const;
    /**
     * @brief Set the animated.
     * @param on on
     * @param speed speed
     */
    void set_animated(bool on, float speed = 3.f);
    /**
     * @brief Set the orientation.
     * @param o New orientation
     */
    void set_orientation(Orientation o);
    /**
     * @brief Set the show label.
     * @param show New show label
     */
    void set_show_label(bool show);

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
    std::string style_key() const override { return "progress_bg"; }

private:
    float _value=0.f,_display=0.f,_speed=3.f;
    bool _animated=true,_label=false;
    Orientation _orient=Orientation::HORIZONTAL;
};
} // namespace titan::ui
