#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/game_component.hpp"

#include "module-render/include/render_layer.hpp"

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"

#include <memory>
#include <optional>
#include <string>

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace sf { class Texture; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Sprite_renderer
// ----------------------------------------------------------------------------

/**
 * @brief Draws a textured sprite at the owner's world transform.
 *
 * Pulls the texture from the Resource_manager by name, builds an sf::Sprite each
 * frame positioned/rotated/scaled by the owner's Transform, and submits it to
 * the renderer on a chosen layer (default ENTITIES). A keepalive sprite is
 * stored so it outlives end_frame() per the renderer's contract.
 *
 *   auto* sr = obj->add_component<Sprite_renderer>();
 *   sr->set_texture("player");
 *   sr->set_origin_centered(true);
 */
class Sprite_renderer final : public Game_component {

public:
    /**
     * @brief Construct an empty sprite renderer (no texture).
     */
    Sprite_renderer() = default;

    /**
     * @brief Set the texture by resource name.
     * @param resource_name Texture key in the Resource_manager
     */
    void set_texture(std::string const& resource_name);

    /**
     * @brief Get the current texture resource name.
     * @return Reference to the texture name string
     */
    std::string const& texture_name() const;

    /**
     * @brief Restrict drawing to a sub-rectangle of the texture (for atlases).
     * @param rect Source rectangle within the texture
     */
    void set_texture_rect(sf::IntRect const rect);

    /**
     * @brief Clear any texture sub-rectangle (draw the whole texture).
     */
    void clear_texture_rect();

    /**
     * @brief Set the sprite tint/color.
     * @param color Tint color (white = untinted)
     */
    void set_color(sf::Color const color);

    /**
     * @brief Get the sprite tint/color.
     * @return Current tint color
     */
    sf::Color color() const;

    /**
     * @brief Center the sprite origin (vs top-left).
     * @param centered True to center the origin
     */
    void set_origin_centered(bool const centered);

    /**
     * @brief Set the render layer for submission.
     * @param layer Render layer
     */
    void set_layer(render::Render_layer const layer);

    /**
     * @brief Set the sort key within the layer (higher draws later).
     * @param key Sort key
     */
    void set_sort_key(float const key);

    /**
     * @brief Acquire the texture from the Resource_manager on start.
     */
    void on_start() override;

    /**
     * @brief Build and submit the sprite for this frame.
     * @param renderer Renderer to submit to
     */
    void render(render::Renderer& renderer) override;

private:
    std::string                  _texture_name; ///< Texture resource key
    std::shared_ptr<sf::Texture> _texture; ///< Resolved texture
    std::optional<sf::IntRect>   _rect; ///< Optional source rect
    sf::Color                    _color = sf::Color::White; ///< Tint color
    bool                         _centered = true; ///< Origin centered?
    render::Render_layer         _layer = render::Render_layer::ENTITIES; ///< Layer
    float                        _sort_key = 0.0f; ///< Sort key in layer

    std::optional<sf::Sprite> _sprite;   ///< Keepalive sprite, rebuilt each frame

    /**
     * @brief Resolve the texture from the Resource_manager by name.
     */
    void _acquire_texture();
};

} // namespace titan::game
