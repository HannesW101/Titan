#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstdint>
#include <string>

namespace titan::ui {

// ============================================================================
// Class Scroll_view
// ----------------------------------------------------------------------------

/** @brief Scrollable viewport. Set content size; children are offset by scroll.
 *  Mouse wheel scrolls while hovered. Draws a thumb on the active axis. */
class Scroll_view final : public Widget {

public:
    enum class Dir { VERTICAL, HORIZONTAL, BOTH };
    /**
     * @brief Construct a Scroll_view.
     * @param name Widget name (unique within its parent)
     */
    explicit Scroll_view(std::string const& name);
    /**
     * @brief Set the dir.
     * @param d New dir
     */
    void set_dir(Dir d);
    /**
     * @brief Set the speed.
     * @param s New speed
     */
    void set_speed(float s);
    /**
     * @brief Set the content size.
     * @param s New content size
     */
    void set_content_size(sf::Vector2f s);
    /**
     * @brief Scroll to.
     * @param off off
     */
    void scroll_to(sf::Vector2f off);
    /**
     * @brief Get the scroll offset.
     * @return The scroll offset
     */
    sf::Vector2f scroll_offset() const;

protected:
    /**
     * @brief Handle the layout event.
     */
    void on_layout() override;
    /**
     * @brief Handle the update event.
     * @param dt dt
     */
    void on_update(float dt) override;
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_arrange_none_children(sf::FloatRect const& inner) override;
    void on_render(render::Renderer& renderer) override;
    void on_pre_child_render(render::Renderer& renderer) override;
    void on_post_child_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "panel"; }

private:
    Dir _dir=Dir::VERTICAL; float _speed=30.f;
    sf::Vector2f _off{0,0},_target{0,0},_content{0,0};
    std::uint64_t _up=0,_down=0;
};
} // namespace titan::ui
