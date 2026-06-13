// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/toggle.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-utils/include/math.hpp"

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;
Toggle::Toggle(std::string const& name) : Widget(name) { enable_click_events(); set_focusable(true); }
void Toggle::set_on(bool on) { _on = on; }
bool Toggle::is_on() const { return _on; }
void Toggle::set_label(std::string const& l) { _label = l; }
void Toggle::set_on_changed(std::function<void(bool)> cb) { _on_changed = std::move(cb); }
void Toggle::on_clicked() { _on = !_on; if (_on_changed) { _on_changed(_on); } }
void Toggle::on_update(float dt) { _knob = lerp(_knob, _on ? 1.f : 0.f, clamp01(dt * 15.f)); }
void Toggle::on_render(render::Renderer& renderer) {
    sf::FloatRect const r = rect();
    render::Render_layer const layer = on_overlay() ? render::Render_layer::UI_OVERLAY : render::Render_layer::UI;
    float const th = r.size.y;
    float const tw = th * 2.f;
    sf::Color const off(60,60,80), on(80,190,120);
    sf::Color track;
    track.r = (std::uint8_t)lerp((float)off.r,(float)on.r,_knob);
    track.g = (std::uint8_t)lerp((float)off.g,(float)on.g,_knob);
    track.b = (std::uint8_t)lerp((float)off.b,(float)on.b,_knob);
    track.a = 255;
    sf::RectangleShape t; t.setPosition(r.position); t.setSize({tw, th}); t.setFillColor(track);
    draw_buffer().add_rect(renderer, layer, std::move(t));
    float const kr = th * 0.4f;
    float const pad = (th - kr*2.f)*0.5f;
    float const x = lerp(pad, tw - pad - kr*2.f, _knob);
    sf::CircleShape knob(kr); knob.setPosition({r.position.x + x, r.position.y + pad}); knob.setFillColor(sf::Color(235,235,240));
    draw_buffer().add_circle(renderer, layer, std::move(knob));
    if (!_label.empty()) {
        auto const& ta = resolved_text_style();
        std::shared_ptr<sf::Font> font;
        try { font = resource_manager().get<sf::Font>(ta.font_name.empty()?"default_font":ta.font_name); }
        catch (std::exception const&) { return; }
        sf::Text txt(*font, _label, static_cast<unsigned int>(ta.size > 0.f ? ta.size : 14.f));
        txt.setFillColor(sf::Color(235,235,240));
        sf::FloatRect const tb = txt.getLocalBounds();
        txt.setPosition({ r.position.x + tw + 8.f, r.position.y + (th - tb.size.y)*0.5f - tb.position.y });
        draw_buffer().add_text(renderer, layer, std::move(txt));
    }
}
} // namespace titan::ui
