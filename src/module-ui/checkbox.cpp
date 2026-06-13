// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/checkbox.hpp"

#include "module-ui/include/ui_manager.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"
#include "module-resources/include/resource_manager.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Checkbox::Checkbox(std::string const& name) : Widget(name) { enable_click_events(); set_focusable(true); }
void Checkbox::set_label(std::string const& l) { _label = l; }
void Checkbox::set_checked(bool c) { _checked = c; }
bool Checkbox::is_checked() const { return _checked; }
void Checkbox::set_on_changed(std::function<void(bool)> cb) { _on_changed = std::move(cb); }
void Checkbox::on_clicked() { _checked = !_checked; if (_on_changed) { _on_changed(_checked); } }
void Checkbox::on_render(render::Renderer& renderer) {
    sf::FloatRect const r = rect();
    float const box = r.size.y;
    render::Render_layer const layer = on_overlay() ? render::Render_layer::UI_OVERLAY : render::Render_layer::UI;
    // box background uses the resolved visual (already drawn by base across full rect),
    // but we want a square box: draw our own check mark inside the leading square.
    auto const& vs = resolved_text_style();
    if (_checked) {
        float const inset = box * 0.25f;
        sf::RectangleShape mark;
        mark.setPosition({ r.position.x + inset, r.position.y + inset });
        mark.setSize({ box - inset*2.f, box - inset*2.f });
        mark.setFillColor(sf::Color(80,190,120));
        draw_buffer().add_rect(renderer, layer, std::move(mark));
    }
    if (!_label.empty()) {
        std::shared_ptr<sf::Font> font;
        try { font = resource_manager().get<sf::Font>(vs.font_name.empty() ? "default_font" : vs.font_name); }
        catch (std::exception const&) { return; }
        sf::Text t(*font, _label, static_cast<unsigned int>(vs.size));
        t.setFillColor(vs.color);
        sf::FloatRect const tb = t.getLocalBounds();
        t.setPosition({ r.position.x + box + 8.f, r.position.y + (box - tb.size.y)*0.5f - tb.position.y });
        draw_buffer().add_text(renderer, layer, std::move(t));
    }
}
} // namespace titan::ui
