#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

namespace titan::ui {

// ============================================================================
// Class Separator
// ----------------------------------------------------------------------------

/** @brief Thin horizontal/vertical divider line. */
class Separator final : public Widget {

public:
    enum class Orientation { HORIZONTAL, VERTICAL };
    /**
     * @brief Construct a Separator.
     * @param name Widget name (unique within its parent)
     */
    explicit Separator(std::string const& name);
    /**
     * @brief Set the orientation.
     * @param o New orientation
     */
    void set_orientation(Orientation o);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "separator"; }
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    Orientation _orient=Orientation::HORIZONTAL;
};
} // namespace titan::ui
