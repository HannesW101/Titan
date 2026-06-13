#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"

#include <vector>
#include <functional>
#include <string>

namespace titan::ui {

// ============================================================================
// Class Minimap
// ----------------------------------------------------------------------------

/** @brief A minimap that maps world coordinates into its rect and draws blips,
 *  plus an optional viewport rectangle. Set the world bounds, then push blips
 *  each frame. Click maps back to world via on_clicked(world_pos). */
class Minimap final : public Widget {

public:
// ============================================================================
// Class Blip
// ----------------------------------------------------------------------------

    struct Blip { sf::Vector2f world; sf::Color color; float radius=2.f; };
    /**
     * @brief Construct a Minimap.
     * @param name Widget name (unique within its parent)
     */
    explicit Minimap(std::string const& name);
    /**
     * @brief Set the world bounds.
     * @param top_left top left
     * @param size size
     */
    void set_world_bounds(sf::Vector2f top_left, sf::Vector2f size);
    /**
     * @brief Clear blips.
     */
    void clear_blips();
    /**
     * @brief Add blip.
     * @param world world
     * @param color color
     * @param radius radius
     */
    void add_blip(sf::Vector2f world, sf::Color color, float radius = 2.f);
    /**
     * @brief Set the viewport.
     * @param world_tl world tl
     * @param world_size world size
     */
    void set_viewport(sf::Vector2f world_tl, sf::Vector2f world_size);
    /**
     * @brief Set the "on clicked" callback.
     * @param cb Callback to invoke
     */
    void set_on_clicked(std::function<void(sf::Vector2f)> cb);

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
    std::string style_key() const override { return "panel"; }

private:
    sf::Vector2f _wtl{0,0},_wsize{1000,1000};
    std::vector<Blip> _blips;
    bool _has_vp=false; sf::Vector2f _vp_tl{0,0},_vp_size{0,0};
    std::function<void(sf::Vector2f)> _on_clicked;
    /**
     * @brief World to local.
     * @param w w
     */
    sf::Vector2f _world_to_local(sf::Vector2f w) const;
};
} // namespace titan::ui
