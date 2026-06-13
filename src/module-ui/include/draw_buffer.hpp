#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/VertexArray.hpp"

#include <deque>
#include <utility>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Draw_buffer
// ----------------------------------------------------------------------------

/**
 * @brief Per-widget persistent storage for drawables submitted each frame.
 *
 * The Renderer requires submitted drawables to remain alive until end_frame().
 * Widgets build transient shapes/text each frame; this buffer owns them for
 * the duration of the frame so the references the Renderer holds stay valid.
 *
 * Call clear() at the start of each render, then use the add_* helpers which
 * store the drawable and submit a stable reference to the Renderer.
 *
 * std::deque is used because it does not invalidate references on push_back,
 * unlike std::vector - critical since the Renderer holds pointers into here.
 */
class Draw_buffer final {

public:
    void clear() {
        _rects.clear();
        _circles.clear();
        _convex.clear();
        _texts.clear();
        _sprites.clear();
        _vertex_arrays.clear();
    }

    void add_rect(
        render::Renderer& r,
        render::Render_layer layer,
        sf::RectangleShape shape,
        float sort_key = 0.0f
        ) {

        _rects.push_back(std::move(shape));
        r.submit(layer, _rects.back(), sf::RenderStates::Default, sort_key);
    }

    void add_circle(
        render::Renderer& r,
        render::Render_layer layer,
        sf::CircleShape shape,
        float sort_key = 0.0f
        ) {

        _circles.push_back(std::move(shape));
        r.submit(layer, _circles.back(), sf::RenderStates::Default, sort_key);
    }

    void add_convex(
        render::Renderer& r,
        render::Render_layer layer,
        sf::ConvexShape shape,
        float sort_key = 0.0f
        ) {

        _convex.push_back(std::move(shape));
        r.submit(layer, _convex.back(), sf::RenderStates::Default, sort_key);
    }

    void add_text(
        render::Renderer& r,
        render::Render_layer layer,
        sf::Text text,
        float sort_key = 0.0f
        ) {

        _texts.push_back(std::move(text));
        r.submit(layer, _texts.back(), sf::RenderStates::Default, sort_key);
    }

    void add_sprite(
        render::Renderer& r,
        render::Render_layer layer,
        sf::Sprite sprite,
        float sort_key = 0.0f
        ) {

        _sprites.push_back(std::move(sprite));
        r.submit(layer, _sprites.back(), sf::RenderStates::Default, sort_key);
    }

    void add_vertex_array(
        render::Renderer& r,
        render::Render_layer layer,
        sf::VertexArray va,
        float sort_key = 0.0f
        ) {

        _vertex_arrays.push_back(std::move(va));
        r.submit(layer, _vertex_arrays.back(), sf::RenderStates::Default, sort_key);
    }

private:
    std::deque<sf::RectangleShape> _rects;
    std::deque<sf::CircleShape>    _circles;
    std::deque<sf::ConvexShape>    _convex;
    std::deque<sf::Text>           _texts;
    std::deque<sf::Sprite>         _sprites;
    std::deque<sf::VertexArray>    _vertex_arrays;
};

} // namespace titan::ui
