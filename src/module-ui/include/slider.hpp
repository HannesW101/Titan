#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <functional>
#include <string>

namespace titan::ui {

// ============================================================================
// Class Slider
// ----------------------------------------------------------------------------

/** @brief Horizontal/vertical value slider with draggable knob. */
class Slider final : public Widget {

public:
    enum class Orientation { HORIZONTAL, VERTICAL };
    /**
     * @brief Construct a Slider.
     * @param name Widget name (unique within its parent)
     */
    explicit Slider(std::string const& name);
    /**
     * @brief Set the range.
     * @param min min
     * @param max max
     */
    void set_range(float min, float max);
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
     * @brief Set the step.
     * @param step New step
     */
    void set_step(float step);
    /**
     * @brief Set the orientation.
     * @param o New orientation
     */
    void set_orientation(Orientation o);
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(float)> cb);

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
     * @brief Handle the released event.
     */
    void on_released() override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "slider_track"; }
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    float _min=0.f,_max=1.f,_value=0.f,_step=0.f;
    Orientation _orient=Orientation::HORIZONTAL;
    bool _drag=false;
    std::function<void(float)> _on_changed;
};

} // namespace titan::ui
