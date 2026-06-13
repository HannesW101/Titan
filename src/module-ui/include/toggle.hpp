#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <functional>
#include <string>

namespace titan::ui {

// ============================================================================
// Class Toggle
// ----------------------------------------------------------------------------

/** @brief Animated on/off switch with a sliding knob and optional label. */
class Toggle final : public Widget {

public:
    /**
     * @brief Construct a Toggle.
     * @param name Widget name (unique within its parent)
     */
    explicit Toggle(std::string const& name);
    /**
     * @brief Set the on.
     * @param on New on
     */
    void set_on(bool on);
    /**
     * @brief Check whether on.
     * @return True if on
     */
    bool is_on() const;
    /**
     * @brief Set the label.
     * @param label New label
     */
    void set_label(std::string const& label);
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(bool)> cb);

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
     * @brief Handle the clicked event.
     */
    void on_clicked() override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "toggle"; }
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    bool  _on = false;
    float _knob = 0.f;
    std::string _label;
    std::function<void(bool)> _on_changed;
};

} // namespace titan::ui
