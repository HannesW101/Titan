// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/icon_button.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Icon_button::Icon_button(std::string const& name) : Widget(name) { enable_click_events(); }
void Icon_button::set_icon(std::string const& t) { _icon = t; }
void Icon_button::set_icon_size(sf::Vector2f s) { _icon_size = s; }
void Icon_button::set_icon_tint(sf::Color c) { _tint = c; }
void Icon_button::set_on_click(std::function<void()> cb) { _on_click = std::move(cb); }
void Icon_button::on_clicked() { if (_on_click) { _on_click(); } }
void Icon_button::on_render(render::Renderer& renderer) {
    if (_icon.empty()) { return; }
    std::shared_ptr<sf::Texture> tex;
    try { tex = resource_manager().get<sf::Texture>(_icon); } catch (std::exception const&) { return; }
    sf::Sprite spr(*tex);
    sf::FloatRect const tb = spr.getLocalBounds();
    if (tb.size.x <= 0.f || tb.size.y <= 0.f) { return; }
    sf::Vector2f ds = _icon_size;
    if (ds.x <= 0.f || ds.y <= 0.f) { ds = tb.size; }
    spr.setScale({ ds.x / tb.size.x, ds.y / tb.size.y });
    sf::FloatRect const r = rect();
    spr.setPosition({ r.position.x + (r.size.x - ds.x)*0.5f, r.position.y + (r.size.y - ds.y)*0.5f });
    spr.setColor(_tint);
    render::Render_layer const layer = on_overlay() ? render::Render_layer::UI_OVERLAY : render::Render_layer::UI;
    draw_buffer().add_sprite(renderer, layer, std::move(spr));
}
} // namespace titan::ui
