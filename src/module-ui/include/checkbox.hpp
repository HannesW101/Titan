#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <functional>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Checkbox
// ----------------------------------------------------------------------------

/** @brief Boolean checkbox with a label drawn to the right of the box. */
class Checkbox final : public Widget {

public:
    /**
     * @brief Construct a Checkbox.
     * @param name Widget name (unique within its parent)
     */
    explicit Checkbox(std::string const& name);
    /**
     * @brief Set the label.
     * @param label New label
     */
    void set_label(std::string const& label);
    /**
     * @brief Set the checked.
     * @param checked New checked
     */
    void set_checked(bool checked);
    /**
     * @brief Check whether checked.
     * @return True if checked
     */
    bool is_checked() const;
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(bool)> cb);

protected:
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
    std::string style_key() const override { return "checkbox"; }

private:
    std::string               _label;
    bool                      _checked = false;
    std::function<void(bool)> _on_changed;
};

} // namespace titan::ui
