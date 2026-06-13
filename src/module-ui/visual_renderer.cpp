// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/visual_renderer.hpp"

#include "module-resources/include/resource_manager.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <algorithm>
#include <memory>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui::visual_renderer {

// ============================================================================
// Helpers
// ----------------------------------------------------------------------------

namespace {

void draw_color(
    Visual const& vis,
    sf::FloatRect const& dst,
    render::Renderer& renderer,
    Draw_buffer& buffer,
    render::Render_layer const layer,
    float const sort_key
    ) {

    sf::RectangleShape rect;
    rect.setPosition(dst.position);
    rect.setSize(dst.size);
    rect.setFillColor(vis.fill_color);
    rect.setOutlineColor(vis.outline_color);
    rect.setOutlineThickness(-vis.outline_thickness);
    buffer.add_rect(renderer, layer, std::move(rect), sort_key);
}

// ----------------------------------------------------------------------------
void draw_texture(
    Visual const& vis,
    sf::FloatRect const& dst,
    render::Renderer& renderer,
    Draw_buffer& buffer,
    resources::Resource_manager& rm,
    render::Render_layer const layer,
    float const sort_key
    ) {

    std::shared_ptr<sf::Texture> tex;
    try {
        tex = rm.get<sf::Texture>(vis.texture_name);
    } catch (std::exception const&) {
        return;
    }

    sf::Sprite spr(*tex);
    if (vis.uv_rect.has_value()) {
        spr.setTextureRect(*vis.uv_rect);
    }

    sf::FloatRect const tb = spr.getLocalBounds();
    if (tb.size.x <= 0.0f || tb.size.y <= 0.0f) { return; }

    float sx = dst.size.x / tb.size.x;
    float sy = dst.size.y / tb.size.y;

    if (vis.preserve_aspect) {
        float const s = std::min(sx, sy);
        sx = s;
        sy = s;
    }

    spr.setScale({ sx, sy });

    float const scaled_w = tb.size.x * sx;
    float const scaled_h = tb.size.y * sy;
    spr.setPosition({
        dst.position.x + (dst.size.x - scaled_w) * 0.5f,
        dst.position.y + (dst.size.y - scaled_h) * 0.5f
    });

    spr.setColor(vis.tint);
    buffer.add_sprite(renderer, layer, std::move(spr), sort_key);
}

// ----------------------------------------------------------------------------
void draw_nine_slice(
    Visual const& vis,
    sf::FloatRect const& dst,
    render::Renderer& renderer,
    Draw_buffer& buffer,
    resources::Resource_manager& rm,
    render::Render_layer const layer,
    float const sort_key
    ) {

    std::shared_ptr<sf::Texture> tex;
    try {
        tex = rm.get<sf::Texture>(vis.texture_name);
    } catch (std::exception const&) {
        return;
    }

    sf::Vector2u const tsz = tex->getSize();
    float const tw = static_cast<float>(tsz.x);
    float const th = static_cast<float>(tsz.y);

    float const sl = vis.border_left;
    float const sr = vis.border_right;
    float const st = vis.border_top;
    float const sb = vis.border_bottom;

    float const src_cx = tw - sl - sr;
    float const src_cy = th - st - sb;

    float const dl = sl, dr = sr, dt = st, db = sb;
    float const dst_cx = dst.size.x - dl - dr;
    float const dst_cy = dst.size.y - dt - db;

    auto piece = [&](
        float src_x, float src_y, float src_w, float src_h,
        float ox, float oy, float ow, float oh
        ) {
        if (src_w <= 0.0f || src_h <= 0.0f || ow <= 0.0f || oh <= 0.0f) { return; }
        sf::Sprite s(*tex);
        s.setTextureRect(sf::IntRect(
            { static_cast<int>(src_x), static_cast<int>(src_y) },
            { static_cast<int>(src_w), static_cast<int>(src_h) }
        ));
        s.setPosition({ dst.position.x + ox, dst.position.y + oy });
        s.setScale({ ow / src_w, oh / src_h });
        s.setColor(vis.tint);
        buffer.add_sprite(renderer, layer, std::move(s), sort_key);
    };

    // 3x3 grid
    piece(0.0f,    0.0f,    sl,     st,     0.0f,        0.0f,        dl,     dt);
    piece(sl,      0.0f,    src_cx, st,     dl,          0.0f,        dst_cx, dt);
    piece(tw - sr, 0.0f,    sr,     st,     dl + dst_cx, 0.0f,        dr,     dt);

    piece(0.0f,    st,      sl,     src_cy, 0.0f,        dt,          dl,     dst_cy);
    piece(sl,      st,      src_cx, src_cy, dl,          dt,          dst_cx, dst_cy);
    piece(tw - sr, st,      sr,     src_cy, dl + dst_cx, dt,          dr,     dst_cy);

    piece(0.0f,    th - sb, sl,     sb,     0.0f,        dt + dst_cy, dl,     db);
    piece(sl,      th - sb, src_cx, sb,     dl,          dt + dst_cy, dst_cx, db);
    piece(tw - sr, th - sb, sr,     sb,     dl + dst_cx, dt + dst_cy, dr,     db);
}

} // anonymous namespace

// ============================================================================
// Public
// ============================================================================

void draw(
    Visual const& vis,
    sf::FloatRect const& dst,
    render::Renderer& renderer,
    Draw_buffer& buffer,
    resources::Resource_manager& rm,
    render::Render_layer const layer,
    float const sort_key
    ) {

    switch (vis.kind) {
        case Visual::Kind::NONE:
            break;
        case Visual::Kind::COLOR:
            draw_color(vis, dst, renderer, buffer, layer, sort_key);
            break;
        case Visual::Kind::TEXTURE:
            draw_texture(vis, dst, renderer, buffer, rm, layer, sort_key);
            break;
        case Visual::Kind::NINE_SLICE:
            draw_nine_slice(vis, dst, renderer, buffer, rm, layer, sort_key);
            break;
        default:
            break;
    }
}

} // namespace titan::ui::visual_renderer
