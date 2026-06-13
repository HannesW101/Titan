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
// Class Resizable_window
// ----------------------------------------------------------------------------

/** @brief Draggable + resizable titled window. Drag title to move, drag the
 *  bottom-right corner handle to resize. */
class Resizable_window final : public Widget {

public:
    /**
     * @brief Construct a Resizable_window.
     * @param name Widget name (unique within its parent)
     */
    explicit Resizable_window(std::string const& name);
    /**
     * @brief Set the title.
     * @param t New title
     */
    void set_title(std::string const& t);
    /**
     * @brief Set the min size.
     * @param s New min size
     */
    void set_min_size(sf::Vector2f s);
    /**
     * @brief Set the title height.
     * @param h New title height
     */
    void set_title_height(float h);

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
    std::string style_key() const override { return "window"; }

private:
    std::string _title; float _title_h=28.f,_handle=14.f; sf::Vector2f _min{120,80};
    bool _drag=false,_resize=false; sf::Vector2f _grab{0,0}, _start_size{0,0};
};
} // namespace titan::ui
