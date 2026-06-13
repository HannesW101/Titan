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
// Class Window
// ----------------------------------------------------------------------------

/** @brief Draggable titled window with a title bar and optional close button.
 *  Children placed inside appear below the title bar (use inner_rect). */
class Window final : public Widget {

public:
    /**
     * @brief Construct a Window.
     * @param name Widget name (unique within its parent)
     */
    explicit Window(std::string const& name);
    /**
     * @brief Set the title.
     * @param t New title
     */
    void set_title(std::string const& t);
    /**
     * @brief Set the closeable.
     * @param c New closeable
     */
    void set_closeable(bool c);
    /**
     * @brief Set the title height.
     * @param h New title height
     */
    void set_title_height(float h);
    /**
     * @brief Set the "on close" callback.
     * @param cb Callback to invoke
     */
    void set_on_close(std::function<void()> cb);

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
    std::string _title; bool _closeable=true,_drag=false; float _title_h=28.f;
    sf::Vector2f _grab{0,0};
    std::function<void()> _on_close;
};
} // namespace titan::ui
