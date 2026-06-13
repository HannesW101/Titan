#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <functional>
#include <string>

namespace titan::ui {

class Label; class Button;
// ============================================================================
// Class Spin_box
// ----------------------------------------------------------------------------

/** @brief Numeric spin edit: [-] [value] [+], all skinnable.
 *  The two buttons and the value label are internal child widgets exposed via
 *  minus_button()/plus_button()/value_field() so you can restyle them (e.g.
 *  give the buttons textures) or reposition them relative to each other. */
class Spin_box final : public Widget {

public:
    /**
     * @brief Construct a Spin_box.
     * @param name Widget name (unique within its parent)
     */
    explicit Spin_box(std::string const& name);
    /**
     * @brief Set the range.
     * @param min min
     * @param max max
     */
    void set_range(float min,float max);
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
     * @brief Set the decimals.
     * @param d New decimals
     */
    void set_decimals(int d);
    /**
     * @brief Set the value width.
     * @param w New value width
     */
    void set_value_width(UI_length w);
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(float)> cb);
    /**
     * @brief Get the minus button.
     * @return The minus button
     */
    Button* minus_button();
    /**
     * @brief Get the plus button.
     * @return The plus button
     */
    Button* plus_button();
    /**
     * @brief Get the value field.
     * @return The value field
     */
    Label*  value_field();

protected:
    /**
     * @brief Handle the layout event.
     */
    void on_layout() override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "panel"; }

private:
    float _val=0.f,_min=0.f,_max=100.f,_step=1.f; int _decimals=0;
    UI_length _value_w = UI_length::px(60.f);
    std::function<void(float)> _on_changed;
    Button* _minus=nullptr; Button* _plus=nullptr; Label* _value_lbl=nullptr;
    bool _built=false;
    /**
     * @brief Build.
     */
    void _build();
    /**
     * @brief Refresh.
     */
    void _refresh();
    /**
     * @brief Step by.
     * @param dir dir
     */
    void _step_by(float dir);
    /**
     * @brief Fmt.
     * @param v v
     */
    std::string _fmt(float v) const;
};
} // namespace titan::ui
