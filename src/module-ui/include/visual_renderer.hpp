#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/visual.hpp"
#include "module-ui/include/draw_buffer.hpp"

#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"

#include "SFML/Graphics/Rect.hpp"

// ============================================================================
// Forward declarations
// ----------------------------------------------------------------------------

namespace titan::resources { class Resource_manager; }

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Visual_renderer
// ----------------------------------------------------------------------------

/**
 * @brief Draws any Visual into a screen rect. Centralizes ALL background
 * drawing so every widget shares identical color/texture/nine-slice logic.
 *
 * Stateless free function group; needs a Draw_buffer to keep drawables alive.
 */
namespace visual_renderer {

/**
 * @brief Draw a visual filling the given rect.
 * @param vis       The visual to draw
 * @param dst       Destination rect in screen pixels
 * @param renderer  Target renderer
 * @param buffer    Frame-persistent storage (drawables outlive the call)
 * @param rm        Resource manager for resolving textures
 * @param layer     Render layer to submit to
 * @param sort_key  Sort key within the layer
 */
void draw(
    Visual const& vis,
    sf::FloatRect const& dst,
    render::Renderer& renderer,
    Draw_buffer& buffer,
    resources::Resource_manager& rm,
    render::Render_layer layer,
    float sort_key = 0.0f
    );

} // namespace visual_renderer

} // namespace titan::ui
