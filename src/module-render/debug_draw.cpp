// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-render/include/debug_draw.hpp"
#include "module-render/include/renderer.hpp"

#include "module-utils/include/math.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::render {

using namespace titan::utils;

// ============================================================================
// Class Debug_draw
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
Debug_draw::Debug_draw(
    Renderer& renderer,
    sf::Font const& font
    )
    : _renderer(renderer)
    , _font(font)
    , _enabled(true)
    , _lines(sf::PrimitiveType::Lines)
    , _filled(sf::PrimitiveType::Triangles)
{}

// ----------------------------------------------------------------------------
void Debug_draw::set_enabled(
    bool const flag
    ) {

    _enabled = flag;
}

// ----------------------------------------------------------------------------
bool Debug_draw::is_enabled() const {

    return _enabled;
}

// ----------------------------------------------------------------------------
void Debug_draw::line(
    sf::Vector2f const a,
    sf::Vector2f const b,
    sf::Color const color
    ) {

    if (!_enabled) { return; }

    _lines.append(sf::Vertex{ .position = a, .color = color });
    _lines.append(sf::Vertex{ .position = b, .color = color });
}

// ----------------------------------------------------------------------------
void Debug_draw::box(
    sf::FloatRect const rect,
    sf::Color const color
    ) {

    if (!_enabled) { return; }

    sf::Vector2f const tl = rect.position;
    sf::Vector2f const tr = { rect.position.x + rect.size.x, rect.position.y };
    sf::Vector2f const br = rect.position + rect.size;
    sf::Vector2f const bl = { rect.position.x, rect.position.y + rect.size.y };

    line(tl, tr, color);
    line(tr, br, color);
    line(br, bl, color);
    line(bl, tl, color);
}

// ----------------------------------------------------------------------------
void Debug_draw::filled_box(
    sf::FloatRect const rect,
    sf::Color const color
    ) {

    if (!_enabled) { return; }

    sf::Vector2f const tl = rect.position;
    sf::Vector2f const tr = { rect.position.x + rect.size.x, rect.position.y };
    sf::Vector2f const br = rect.position + rect.size;
    sf::Vector2f const bl = { rect.position.x, rect.position.y + rect.size.y };

    _filled.append(sf::Vertex{ .position = tl, .color = color });
    _filled.append(sf::Vertex{ .position = tr, .color = color });
    _filled.append(sf::Vertex{ .position = br, .color = color });

    _filled.append(sf::Vertex{ .position = tl, .color = color });
    _filled.append(sf::Vertex{ .position = br, .color = color });
    _filled.append(sf::Vertex{ .position = bl, .color = color });
}

// ----------------------------------------------------------------------------
void Debug_draw::circle(
    sf::Vector2f const center,
    float const radius,
    sf::Color const color,
    int const segments
    ) {

    if (!_enabled) { return; }

    float const step = TAU / static_cast<float>(segments);

    for (int i = 0; i < segments; ++i) {

        float const a0 = static_cast<float>(i)     * step;
        float const a1 = static_cast<float>(i + 1) * step;

        sf::Vector2f const p0 = center + from_angle(a0, radius);
        sf::Vector2f const p1 = center + from_angle(a1, radius);

        line(p0, p1, color);
    }
}

// ----------------------------------------------------------------------------
void Debug_draw::cross(
    sf::Vector2f const center,
    float const radius,
    sf::Color const color
    ) {

    if (!_enabled) { return; }

    line({ center.x - radius, center.y }, { center.x + radius, center.y }, color);
    line({ center.x, center.y - radius }, { center.x, center.y + radius }, color);
}

// ----------------------------------------------------------------------------
void Debug_draw::arrow(
    sf::Vector2f const from,
    sf::Vector2f const to,
    sf::Color const color
    ) {

    if (!_enabled) { return; }

    line(from, to, color);

    sf::Vector2f const dir = to - from;
    float const len        = length(dir);

    if (len < EPSILON) { return; }

    sf::Vector2f const norm = normalize(dir);
    float const head_size   = std::min(len * 0.2f, 10.0f);
    sf::Vector2f const perp = perpendicular(norm);

    sf::Vector2f const tip_l = to - norm * head_size + perp * (head_size * 0.5f);
    sf::Vector2f const tip_r = to - norm * head_size - perp * (head_size * 0.5f);

    line(to, tip_l, color);
    line(to, tip_r, color);
}

// ----------------------------------------------------------------------------
void Debug_draw::text(
    sf::Vector2f const position,
    std::string const& message,
    sf::Color const color,
    unsigned int const char_size
    ) {

    if (!_enabled) { return; }

    sf::Text t(_font, message, char_size);
    t.setPosition(position);
    t.setFillColor(color);

    _texts.push_back(std::move(t));
}

// ----------------------------------------------------------------------------
void Debug_draw::flush() {

    if (!_enabled) { return; }

    if (_lines.getVertexCount() > 0u) {

        _renderer.submit(Render_layer::DEBUG, _lines);
    }

    if (_filled.getVertexCount() > 0u) {

        _renderer.submit(Render_layer::DEBUG, _filled);
    }

    for (auto const& t : _texts) {

        _renderer.submit(Render_layer::DEBUG, t);
    }

    clear();
}

// ----------------------------------------------------------------------------
void Debug_draw::clear() {

    _lines.clear();
    _filled.clear();
    _texts.clear();
}

} // namespace titan::render
