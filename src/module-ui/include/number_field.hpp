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
// Class Number_field
// ----------------------------------------------------------------------------

/** @brief Numeric text entry with min/max clamping and decimals. Accepts only
 *  digits, '-', and '.'. Commits on Enter or focus loss. */
class Number_field final : public Widget {

public:
    /**
     * @brief Construct a Number_field.
     * @param name Widget name (unique within its parent)
     */
    explicit Number_field(std::string const& name);
    /**
     * @brief Set the range.
     * @param min min
     * @param max max
     */
    void set_range(double min,double max);
    /**
     * @brief Set the value.
     * @param v New value
     */
    void set_value(double v);
    /**
     * @brief Get the value.
     * @return The value
     */
    double value() const;
    /**
     * @brief Set the decimals.
     * @param d New decimals
     */
    void set_decimals(int d);
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(double)> cb);

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
    std::string _buf; double _val=0,_min=-1e18,_max=1e18; int _decimals=0; float _blink=0.f;
    std::function<void(double)> _on_changed;
    std::uint64_t _text=0,_enter=0,_bs=0;
    /**
     * @brief Commit.
     */
    void _commit();
    /**
     * @brief Get the fmt.
     * @return The fmt
     */
    std::string _fmt() const;
};
} // namespace titan::ui
