// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/button.hpp"

#include "module-render/include/renderer.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

Button::Button(std::string const& name) : Widget(name) { enable_click_events(); set_focusable(true); }
void Button::set_text(std::string const& t) { _text = t; }
std::string const& Button::text() const { return _text; }
void Button::set_on_click(std::function<void()> cb) { _on_click = std::move(cb); }
void Button::on_clicked() { if (_on_click) { _on_click(); } }
void Button::on_render(render::Renderer& renderer) {
    draw_text(renderer, rect(), _text, Text_align::CENTER);
}
} // namespace titan::ui
