#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <string>

namespace titan::ui {

// ============================================================================
// Class Nine_slice_panel
// ----------------------------------------------------------------------------

/** @brief A panel whose background is a nine-slice texture. */
class Nine_slice_panel final : public Widget {

public:
    /**
     * @brief Construct a Nine_slice_panel.
     * @param name Widget name (unique within its parent)
     */
    explicit Nine_slice_panel(std::string const& name);
    /**
     * @brief Set the texture.
     * @param resource_name New texture
     */
    void set_texture(std::string const& resource_name);
    /**
     * @brief Set the border.
     * @param top top
     * @param right right
     * @param bottom bottom
     * @param left left
     */
    void set_border(float top,float right,float bottom,float left);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    std::string _tex; float _t=8,_r=8,_b=8,_l=8;
};
} // namespace titan::ui
